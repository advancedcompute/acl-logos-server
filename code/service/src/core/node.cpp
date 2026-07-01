#include "core/node.h"

#include <grpc++/server_builder.h>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"

#include "core/rpc/services/status_service.h"

#include "file.h"
#include "string_helpers.h"

#include "soci/mysql/soci-mysql.h"
#include "soci/postgresql/soci-postgresql.h"
#include "soci/sqlite3/soci-sqlite3.h"

namespace acl { namespace logos { namespace core {

    bool BlockchainNode::Initialize(const LogosSvcSettings& settings)
    {
        if(!_initialized)
        {
            _initialized = initializeLogging(settings) &&
                initializeDatabase(settings)    &&
                //initializeDatastorage(settings)   &&
                //initializeMessaging(settings)    &&
                initializeGRPCServer(settings);
            
                if(_initialized)
                    _settings = settings;
        } else {
            std::cout << " - BlockchainNode already initialized\n";
        }
        return _initialized;
    }
    
    void BlockchainNode::Run()
    {
        if(_initialized) {
            if(_serverInstance == nullptr) {
                LogMessage("Starting up new blockchain");

                std::stringstream ss;
                ss << _settings.grpc_settings.address << ":" << _settings.grpc_settings.port;

                grpc::ServerBuilder builder;
                if(_initialized) {
                    auto serverAddress = ss.str();
                    builder.AddListeningPort(serverAddress, _credentials);

                    for(auto service : _nodeServiceVect) {
                        if(service && service->RPCService())
                        {
                            builder.RegisterService(service->RPCService());
                        }
                    }

                    grpc::ResourceQuota resourceQuota;
                    resourceQuota.SetMaxThreads(_settings.grpc_settings.max_thread_count);
                    builder.SetResourceQuota(resourceQuota);

                    //printf("Listening: %s\n", serverAddress.c_str());
                    LogMessage(cpp::utils::stringFormat("GRPC server listening on %d threads at %s:%d", 
                        _settings.grpc_settings.max_thread_count, _settings.grpc_settings.address.c_str(),
                        _settings.grpc_settings.port));

                    _grpcServerThread = std::thread([&]() {
                        _serverInstance = builder.BuildAndStart();
                        _serverInstance->Wait();
                    });
                    _grpcServerThread.join();
                } else {
                    LogMessage("Server has not been initialized. Shutting down...");
                }
            } else {
                LogMessage("Server instance already exists, can't have more than one at the same time");
            }
        } else {
            LogMessage("Blockchain node has not been initialized");
        }
    }

    void BlockchainNode::Shutdown()
    {
        _nodeServiceVect.clear();
        _nodeServiceMap.clear();

        LogMessage("Shutting down node");
        if(_serverInstance != nullptr)
        {
            _serverInstance->Shutdown();
        }

        _sqlSession.reset();
        _loggers.clear();
    }

    bool BlockchainNode::initializeGRPCServer(const LogosSvcSettings& settings)
    {
        // 1. Load TLS stuff
        auto initTLS = false;
        if(settings.grpc_settings.tls.use_tls)
        {
            std::string certPem, keyPem;
            if(cpp::utils::read_file_contents(settings.grpc_settings.tls.cert_path, certPem) &&
                cpp::utils::read_file_contents(settings.grpc_settings.tls.key_path, keyPem))
            {
                //grpc::SslServerCredentialsOptions sslCertOptions(GRPC_SSL_REQUEST_AND_REQUIRE_CLIENT_CERTIFICATE_AND_VERIFY);
                grpc::SslServerCredentialsOptions sslCertOptions(GRPC_SSL_REQUEST_CLIENT_CERTIFICATE_BUT_DONT_VERIFY);
                grpc::SslServerCredentialsOptions::PemKeyCertPair keyCertPair = {
                    keyPem, certPem
                };

                sslCertOptions.force_client_auth = false;
                sslCertOptions.pem_key_cert_pairs.push_back(keyCertPair);
                _credentials = grpc::SslServerCredentials(sslCertOptions);

                LogMessage("Successfully loaded certificate and key from files");
                initTLS = true;
            } else {
                LogMessage("Failed to open TLS cert and key files. Review your settings", spdlog::level::err);
            }
        } else {
            LogMessage("GRPC server will run with TLS disabled", spdlog::level::warn);
            _credentials = grpc::InsecureServerCredentials();
            initTLS = true;
        }

        // 2. Load services
        auto initServices = initializeGRPCServices(settings);
        return initTLS && initServices;
    }

    bool BlockchainNode::initializeGRPCServices(const LogosSvcSettings& settings)
    {
        _nodeServiceVect.clear();
        _nodeServiceMap.clear();

        auto statusService = std::shared_ptr<acl::logos::core::rpc::StatusService>(new acl::logos::core::rpc::StatusService);
        _nodeServiceVect.push_back(statusService);
        _nodeServiceMap["status"] = statusService;

        return true;
    }

    bool BlockchainNode::initializeLogging(const LogosSvcSettings& settings)
    {
        _loggers.clear();
        for(const auto& logSettings : settings.log_settings)
        {
            std::shared_ptr<spdlog::logger> logPtr;
            if(logSettings.output == "console")
            {
                logPtr = spdlog::stdout_color_mt(logSettings.name, spdlog::color_mode::always);
            } else if(logSettings.output == "rotating-file")
            {
                auto fully_qualified_output_path = cpp::utils::full_resolve_path(logSettings.output_path);
                logPtr = spdlog::rotating_logger_mt(logSettings.name,
                    fully_qualified_output_path, logSettings.max_size, logSettings.max_files);
            } else if(logSettings.output == "daily-file")
            {
                auto fully_qualified_output_path = cpp::utils::full_resolve_path(logSettings.output_path);
                // TODO: Convert logSettings.restart_time from 24H clock (HH:MM) to ints.
                // For now, just hard-coding it for 00:00 (midnight).
                logPtr = spdlog::daily_logger_mt(logSettings.name, fully_qualified_output_path, 0, 0, false, logSettings.max_files);
            }

            if(logSettings.level == "trace") {
                logPtr->set_level(spdlog::level::trace);
            } else if(logSettings.level == "debug") {
                logPtr->set_level(spdlog::level::debug);
            } else if(logSettings.level == "info") {
                logPtr->set_level(spdlog::level::info);
            } else if(logSettings.level == "warn") {
                logPtr->set_level(spdlog::level::warn);
            } else if(logSettings.level == "error") {
                logPtr->set_level(spdlog::level::err);
            } else if(logSettings.level == "critical") {
                logPtr->set_level(spdlog::level::critical);
            }
            logPtr->set_pattern(logSettings.pattern);
            _loggers.push_back(logPtr);
        }

        if(_loggers.size() == settings.log_settings.size()) {
            LogMessage("Loggers have been successfully initialized");
            return true;
        } else {
            LogMessage("Loggers have been initialized", spdlog::level::warn);
            return false;
        }
    }

    bool BlockchainNode::initializeDatabase(const LogosSvcSettings& settings)
    {
        // 1. Connect to databases
        auto success = false;
        std::stringstream ss;
        ss << "host=" << settings.database_settings.host << " ";
        ss << "port=" << settings.database_settings.port << " ";
        ss << "db=" << settings.database_settings.database << " ";
        ss << "user=" << settings.database_settings.username << " ";
        ss << "password='" << settings.database_settings.password << "' ";

        if(settings.database_settings.certificate.use_tls)
        {
            ss << "sslcert=" << cpp::utils::full_resolve_path(settings.database_settings.certificate.cert_path).c_str() << " ";
            ss << "sslkey=" << cpp::utils::full_resolve_path(settings.database_settings.certificate.key_path).c_str() << " ";
            ss << "sslca=" << cpp::utils::full_resolve_path(settings.database_settings.certificate.ca_path).c_str() << " ";
            ss << "ssl_mode=REQUIRED ";        // TODO: Make this customizable. For now, good enough.
        } else {
            LogMessage("Database connection is unencrypted. Recommended user turns on encryption", spdlog::level::warn);
        }

        if(settings.database_settings.engine == "mysql") {
            try {
                _sqlSession = std::make_shared<soci::session>(
                    soci::session(soci::mysql, ss.str())
                );
                success = true;
            } catch(soci::mysql_soci_error& err) {
                LogMessage(cpp::utils::stringFormat("Failed to connect to database: %s", err.what()), spdlog::level::err);
            }

        } else if(settings.database_settings.engine == "postgresql") {
            try {
                _sqlSession = std::make_shared<soci::session>(
                    soci::session(soci::postgresql, ss.str())
                );
                success = true;
            } catch(soci::postgresql_soci_error& err) {
                LogMessage(cpp::utils::stringFormat("Failed to connect to database: %s", err.what()), spdlog::level::err);
            }
        }  else if(settings.database_settings.engine == "sqlite3") {
            try {
                _sqlSession = std::make_shared<soci::session>(
                    soci::session(soci::sqlite3, ss.str())
                );
                success = true;
            } catch(soci::sqlite3_soci_error& err) {
                LogMessage(cpp::utils::stringFormat("Failed to connect to database: %s", err.what()), spdlog::level::err);
            }
        }

        if(success) {
            // Connected to database, now need to check schema, make tables etc.
            // 2. Resolve schema - create tables if need be
            LogMessage("Successfully connected to database, now checking schema");
            
        } else {
            LogMessage(cpp::utils::stringFormat("Unable to connect to database host: %s",
                settings.database_settings.host), spdlog::level::err);
        }
        return success;
    }

    bool BlockchainNode::initializeDatastorage(const LogosSvcSettings& settings)
    {
        // TODO
        // 1. Local file IO
        // 2. GCloud Storage
        // 3. Torrent P2P Storage
        // 4. IPFS?
        return false;
    }
    
    bool BlockchainNode::initializeMessaging(const LogosSvcSettings& settings)
    {
        // TODO
        // 1. Create Kafka objects.
        // 2. Use kafka config
        // 3. Report back status
        return false;
    }


}}}