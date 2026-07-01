
#include "core/config/cmd_arg_validator.h"
#include "file.h"
#include "string_helpers.h"
#include "certificate.h"
//#include "certificate_builder.h"

namespace acl { namespace logos { namespace core {

    int LogosSvcSettingsValidator::validateLogSettings(LogosSvcSettings& config)
    {

        int errorCount = 0;

        std::vector<std::string> permittedLogLevels = {
            "trace", "debug", "info", "warn", "error", "critical"
        };

        std::vector<std::string> permittedOutputTypes = {
            "console", "rotating-file", "daily-file"
        };

        // Validating log settings
        for(const auto& logConfig: config.log_settings)
        {
            if(logConfig.name.empty())
            {
                getCallback()("log.name", "<empty>", "Log name must be set");
                ++errorCount;
            }

            if(logConfig.level.empty())
            {
                getCallback()("log.level", "<empty>", "Log level must be set");
                ++errorCount;
            } else {
                auto log_level_it = std::find(permittedLogLevels.begin(), permittedLogLevels.end(), logConfig.level);
                if(log_level_it == permittedLogLevels.end()) {
                    getCallback()("log.level", logConfig.level,
                        cpp::utils::stringFormat("Log level is not permitted.\n Value must be one of: %s",
                            cpp::utils::listConcat(permittedLogLevels, ", ", "", ""))
                    );
                    ++errorCount;
                }
            }

            if(logConfig.output.empty())
            {
                getCallback()("log.output", "<empty>", "Log output must be set");
                ++errorCount;
            } else {
                auto log_output_it = std::find(permittedOutputTypes.begin(), permittedOutputTypes.end(), logConfig.output);
                if(log_output_it == permittedOutputTypes.end()) { 
                    getCallback()("log.output", logConfig.output,
                        cpp::utils::stringFormat("Log output is not permitted.\n Value must be one of: %s",
                            cpp::utils::listConcat(permittedOutputTypes, ", ", "", ""))
                    );
                    ++errorCount;
                }
            }

            if(logConfig.pattern.empty())
            {
                getCallback()("log.pattern", "<empty>", "Log pattern must be set");
                ++errorCount;
            }

            if(logConfig.output == "rotating-file" || logConfig.output == "daily-file")
            {
                if(logConfig.output_path.empty())
                {
                    getCallback()("log.output-path", "<empty>", "Log output-path cannot be empty for rotating-file or daily-file");
                    ++errorCount;
                } else {
                    auto log_output_path = cpp::utils::full_resolve_path(logConfig.output_path);
                    cpp::utils::create_directories(log_output_path.parent_path());
                }
            }

            if(logConfig.output == "rotating-file")
            {
                if(logConfig.max_size <= 0)
                {
                    getCallback()("log.max-size", std::to_string(logConfig.max_size), "Log max_size must be a positive integer");
                    ++errorCount;
                }

                if(logConfig.max_files <= 0)
                {
                    getCallback()("log.max-files", std::to_string(logConfig.max_files), "Log max_files must be a positive integer");
                    ++errorCount;
                }
            }

            if(logConfig.output == "daily-file")
            {
                if(logConfig.restart_time.empty())
                {
                    getCallback()("log.restart-time", "<empty>", "Log restart time must be specified for daily logs");
                    ++errorCount;
                }
            }

        }

        return errorCount;
    }

    int LogosSvcSettingsValidator::validateDatabaseSettings(LogosSvcSettings& config)
    {
        int errorCount = 0;

        std::vector<std::string> permittedDatabaseEngines = {
            "mysql", "postgresql", "sqlite3"
        };

        // Database settings
        if(config.database_settings.engine.empty())
        {
            getCallback()("database.engine", "<empty>", "Database engine must be set");
            ++errorCount;
        } else {
            auto found_db_engine_it = std::find(permittedDatabaseEngines.begin(), permittedDatabaseEngines.end(), config.database_settings.engine);
            if(found_db_engine_it == permittedDatabaseEngines.end())
            {
                getCallback()("database.engine", config.database_settings.engine,
                        cpp::utils::stringFormat("Database engine must be a supported value.\n Supported values are: %s",
                            cpp::utils::listConcat(permittedDatabaseEngines, ", ", "", ""))
                    );
                    ++errorCount;
            }
        }

        if(config.database_settings.host.empty())
        {
            getCallback()("database.host", "<empty>", "Database host must be set");
            ++errorCount;
        }

        if(config.database_settings.port <= 0)
        {
            getCallback()("database.port", std::to_string(config.database_settings.port), "Database port must be a positive integer");
            ++errorCount;
        }

        if(config.database_settings.certificate.use_tls)
        {
            if(config.database_settings.certificate.ca_path.empty())
            {
                getCallback()("database.certificate.ca_path", "<empty>", "Database certificate ca_path must be set");
                ++errorCount;
            } else {
                cpp::utils::Certificate cert;
                if(!cert.load_pem_file(config.database_settings.certificate.ca_path)) {
                    getCallback()("database.certificate.ca_path", "<Loading from disk>", "Failed to load certificate CA file from disk");
                    ++errorCount;
                }
            }

            if(config.database_settings.certificate.cert_path.empty())
            {
                getCallback()("database.certificate.cert_path", "<empty>", "Database certificate cert_path must be set");
                ++errorCount;
            } else {
                cpp::utils::Certificate cert;
                if(!cert.load_pem_file(config.database_settings.certificate.cert_path)) {
                    getCallback()("database.certificate.cert_path", "<Loading from disk>", "Failed to load certificate cert file from disk");
                    ++errorCount;
                }
            }

            if(config.database_settings.certificate.key_path.empty())
            {
                getCallback()("database.certificate.key_path", "<empty>", "Database certificate key_path must be set");
                ++errorCount;
            } else {
                cpp::utils::Certificate cert;

                //printf("config.database_settings.certificate.key_path: %s\n", config.database_settings.certificate.key_path.c_str());
                if(!cert.load_pem_file(config.database_settings.certificate.key_path)) {
                    //getCallback()("database.certificate.key_path", "<Loading from disk>", "Failed to load certificate key file from disk");
                    //++errorCount;
                }
            }
        }

        return errorCount;
    }

    int LogosSvcSettingsValidator::validateIdentitySettings(LogosSvcSettings& config)
    {
        int errorCount = 0;

        if(config.identity_settings.node_id.empty())
        {
            getCallback()("identity.node_id", "<empty>", "Identity node ID must be set");
            ++errorCount;
        }

        if(config.identity_settings.operator_id.empty())
        {
            getCallback()("identity.operator_id", "<empty>", "Identity operator ID must be set");
            ++errorCount;
        }

        if(!config.identity_settings.keypair.type.empty())
        {
            auto kp_type_it = std::find(permittedKeyTypes.begin(), permittedKeyTypes.end(), config.identity_settings.keypair.type);
            if(kp_type_it == permittedKeyTypes.end())
            {
                getCallback()("identity.keypair.type", config.identity_settings.keypair.type,
                        cpp::utils::stringFormat("Identity keypair type must be a supported value.\n Supported values are: %s",
                            cpp::utils::listConcat(permittedKeyTypes, ", ", "", ""))
                    );
                    ++errorCount;
            }
        } else {
            config.identity_settings.keypair.type = "ecc";   // Default to ECC
        }

        if(!config.identity_settings.keypair.public_key_path.empty())
        {
            id_key_type = config.identity_settings.keypair.type;    // Save for later use
            if(config.identity_settings.keypair.type == "ecc") {
                errorCount += validateECCIdentityKey(config, identity_ecc_key);
            } else if(config.identity_settings.keypair.type == "ed25519") {
                errorCount += validateEDIdentityKey(config, identity_ed_key);
            } else if(config.identity_settings.keypair.type == "rsa") {
                errorCount += validateRSAIdentityKey(config, identity_rsa_key);
            }
        } else {
            getCallback()("identity.keypair.public_key_path", "<empty>", "Identity keypair public_key_path must be set");
            ++errorCount;
        }

        if(!config.identity_settings.keypair.private_key_path.empty())
        {
            // TODO: Load keys
        } else {
            getCallback()("identity.keypair.private_key_path", "<empty>", "Identity keypair private_key_path must be set");
            ++errorCount;
        }

        
        if(config.identity_settings.certificate.use_tls)
        {
            if(config.identity_settings.certificate.ca_path.empty())
            {
                getCallback()("identity.certificate.ca_path", "<empty>", "Identity certificate ca_path must be set");
                ++errorCount;
            } else {
                cpp::utils::Certificate cert;
                if(!cert.load_pem_file(config.identity_settings.certificate.ca_path)) {
                    getCallback()("identity.certificate.ca_path", "<Loading from disk>", "Failed to load certificate CA file from disk");
                    ++errorCount;
                }
            }

            if(config.identity_settings.certificate.cert_path.empty())
            {
                getCallback()("identity.certificate.cert_path", "<empty>", "Identity keypair cert_path must be set");
                ++errorCount;
            } else {
                cpp::utils::Certificate cert;
                if(!cert.load_pem_file(config.identity_settings.certificate.cert_path)) {
                    getCallback()("identity.certificate.cert_path", "<Loading from disk>", "Failed to load certificate cert file from disk");
                    ++errorCount;
                }
            }

            if(config.identity_settings.certificate.key_path.empty())
            {
                getCallback()("identity.certificate.key_path", "<empty>", "Identity keypair key_path must be set");
                ++errorCount;
            } else {
                cpp::utils::Certificate cert;
                if(!cert.load_pem_file(config.identity_settings.certificate.cert_path)) {
                    getCallback()("identity.certificate.key_path", "<Loading from disk>", "Failed to load certificate key file from disk");
                    ++errorCount;
                }
            }
        }

        return errorCount;
    }

    int LogosSvcSettingsValidator::validateGrpcSettings(LogosSvcSettings& config)
    {
        int errorCount = 0;
        if(config.grpc_settings.address.empty()) {
            getCallback()("grpc.address", "<empty>", "GRPC address must be set");
            ++errorCount;
        }

        // TODO: Validate other fields
        if(config.grpc_settings.tls.use_tls) {
            if(config.grpc_settings.tls.key_path.empty())
            {
                getCallback()("grpc.certificate.key_path", "<empty>", "Grpc certificate key_path must be set");
                ++errorCount;
            } else {
                cpp::utils::Certificate cert;
                if(!cert.load_pem_file(config.grpc_settings.tls.key_path)) {
                    // TODO: Build certificate and save to disk
                    //getCallback()("grpc.certificate.key_path", "<Loading from disk>", "Failed to load certificate key file from disk");
                    //++errorCount;
                }
            }

            if(config.grpc_settings.tls.cert_path.empty())
            {
                getCallback()("grpc.certificate.cert_path", "<empty>", "Certificate cert_path must be set");
                ++errorCount;
            } else {
                cpp::utils::Certificate cert;
                if(!cert.load_pem_file(config.grpc_settings.tls.cert_path)) {
                    // TODO: Build certificate and save to disk

                    //getCallback()("grpc.certificate.cert_path", "<Loading from disk>", "Failed to load certificate cert file from disk");
                    //++errorCount;
                }
            }
        }


        return errorCount;
    }


    int LogosSvcSettingsValidator::validateECCIdentityKey(LogosSvcSettings& config, cpp::utils::ECC& key)
    {
        int errorCount = 0;

        if(cpp::utils::file_exists(config.identity_settings.keypair.private_key_path) &&
            cpp::utils::file_exists(config.identity_settings.keypair.public_key_path))
        {
            if(!key.load_own_private_key_from_pem_file(config.identity_settings.keypair.private_key_path))
            {
                getCallback()("identity.keypair.private_key_path", config.identity_settings.keypair.private_key_path, \
                    "Failed to load private key from file");
                ++errorCount;
            }

            if(!key.load_own_public_key_from_pem_file(config.identity_settings.keypair.public_key_path))
            {
                getCallback()("identity.keypair.public_key_path", config.identity_settings.keypair.public_key_path, \
                    "Failed to load public key from file");
                ++errorCount;
            }
        } else {
            auto privkey_path = cpp::utils::full_resolve_path(config.identity_settings.keypair.private_key_path);
            auto pubkey_path = cpp::utils::full_resolve_path(config.identity_settings.keypair.public_key_path);

            cpp::utils::create_directories(privkey_path.parent_path());
            cpp::utils::create_directories(pubkey_path.parent_path());

            config.identity_settings.keypair.private_key_path = privkey_path;
            config.identity_settings.keypair.public_key_path = pubkey_path;
                    
            if( !(key.generate_own_keypair() && key.export_private_key(config.identity_settings.keypair.private_key_path) && 
                key.export_public_key(config.identity_settings.keypair.public_key_path)) )
            {
                // Error
                getCallback()("identity.keypair", "<Saving to disk>", "Failed to save public and private keypair files to disk");
                ++errorCount;
            }
        }
        return errorCount;
    }

    int LogosSvcSettingsValidator::validateEDIdentityKey(LogosSvcSettings& config, cpp::utils::ED25519& key)
    {
        int errorCount = 0;
        if(cpp::utils::file_exists(config.identity_settings.keypair.private_key_path) &&
            cpp::utils::file_exists(config.identity_settings.keypair.public_key_path))
        {
            if(!key.import_private_key(config.identity_settings.keypair.private_key_path))
            {
                getCallback()("identity.keypair.private_key_path", config.identity_settings.keypair.private_key_path, \
                    "Failed to load private key from file");
                ++errorCount;
            }

            if(!key.import_public_key(config.identity_settings.keypair.public_key_path))
            {
                getCallback()("identity.keypair.public_key_path", config.identity_settings.keypair.public_key_path, \
                    "Failed to load public key from file");
                ++errorCount;
            }
        } else {
            auto privkey_path = cpp::utils::full_resolve_path(config.identity_settings.keypair.private_key_path);
            auto pubkey_path = cpp::utils::full_resolve_path(config.identity_settings.keypair.public_key_path);

            cpp::utils::create_directories(privkey_path.parent_path());
            cpp::utils::create_directories(pubkey_path.parent_path());

            config.identity_settings.keypair.private_key_path = privkey_path;
            config.identity_settings.keypair.public_key_path = pubkey_path;
                    
            if( !(key.generate_keypair() && key.export_private_key(config.identity_settings.keypair.private_key_path) &&
                key.export_public_key(config.identity_settings.keypair.public_key_path)) )
            {
                // Error
                getCallback()("identity.keypair", "<Saving to disk>", "Failed to save public and private keypair files to disk");
                ++errorCount;
            }
        }
        return errorCount;
    }

    int LogosSvcSettingsValidator::validateRSAIdentityKey(LogosSvcSettings& config, cpp::utils::RSA& key)
    {
        int errorCount = 0;

        if(cpp::utils::file_exists(config.identity_settings.keypair.private_key_path) &&
            cpp::utils::file_exists(config.identity_settings.keypair.public_key_path))
        {
            if(!key.import_private_key(config.identity_settings.keypair.private_key_path))
            {
                getCallback()("identity.keypair.private_key_path", config.identity_settings.keypair.private_key_path, \
                    "Failed to load private key from file");
                ++errorCount;
            }

            if(!key.import_public_key(config.identity_settings.keypair.public_key_path))
            {
                getCallback()("identity.keypair.public_key_path", config.identity_settings.keypair.public_key_path, \
                    "Failed to load public key from file");
                ++errorCount;
            }
        } else {
            auto privkey_path = cpp::utils::full_resolve_path(config.identity_settings.keypair.private_key_path);
            auto pubkey_path = cpp::utils::full_resolve_path(config.identity_settings.keypair.public_key_path);

            cpp::utils::create_directories(privkey_path.parent_path());
            cpp::utils::create_directories(pubkey_path.parent_path());

            config.identity_settings.keypair.private_key_path = privkey_path;
            config.identity_settings.keypair.public_key_path = pubkey_path;
                    
            if( !(key.generate_keypair() && key.export_private_key(config.identity_settings.keypair.private_key_path) &&
                key.export_public_key(config.identity_settings.keypair.public_key_path)) )
            {
                // Error
                getCallback()("identity.keypair", "<Saving to disk>", "Failed to save public and private keypair files to disk");
                ++errorCount;
            }
        }
        return errorCount;
    }


    int LogosSvcSettingsValidator::validate(LogosSvcSettings& config)
    {
        int errorCount = 0;
        
        errorCount += validateLogSettings(config);
        errorCount += validateIdentitySettings(config);
        errorCount += validateDatabaseSettings(config);
        errorCount += validateGrpcSettings(config);

        return errorCount;
    }


}}}