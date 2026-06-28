#pragma once

#include "core/config/cmd_arg_options.h"
#include "core/interfaces/blockchain_node.h"

#include "spdlog/logger.h"

#include <grpc++/server.h>
//#include <grpc++/server_builder.h>
#include <grpc++/security/server_credentials.h>
#include <librdkafka/rdkafkacpp.h>

#include "soci/soci.h"

#include <memory>
#include <vector>
#include <map>

namespace acl { namespace logos { namespace core {

    // ENDS Callback //
    class EventNotificationDeliveryServiceCb : public RdKafka::DeliveryReportCb {
        public:
          void dr_cb(RdKafka::Message &message) override {
              if(message.err()) {
                  //syslog(LOG_ERR, "Error sending message to distribution/notification server: %s", message.errstr().c_str());
              } else {
                  //syslog(LOG_DEBUG, "Successfully delivered message to topic '%s' [%d] at offset %ld", \
                  //  message.topic_name().c_str(), message.partition(), message.offset());
              }
          }
    };


    class BlockchainNode: public iblockchain_node
    {
        public:
            BlockchainNode() = default;
            virtual ~BlockchainNode() {
                Shutdown();
            }

            
            bool Initialize(const LogosSvcSettings& settings);
            void Run();
            void Shutdown();

            const std::vector<std::shared_ptr<spdlog::logger>> Loggers() { return _loggers; }


            void LogMessage(const std::string& msg, spdlog::level::level_enum level = spdlog::level::info) {
                for(auto& logger : Loggers())
                {
                    switch(level)
                    {
                        case spdlog::level::critical:
                            logger->critical(msg); break;
                        case spdlog::level::err:
                            logger->error(msg); break;
                        case spdlog::level::warn:
                            logger->warn(msg); break;
                        case spdlog::level::info:
                            logger->info(msg); break;
                        case spdlog::level::debug:
                            logger->debug(msg); break;
                        case spdlog::level::trace:
                            logger->trace(msg); break;
                    }
                }
            }


        private:
            bool initializeGRPCServer(const LogosSvcSettings& settings);
            bool initializeGRPCServices(const LogosSvcSettings& settings);

            //bool initializeBlockchainIdentity(const LogosSvcSettings& settings);
            //bool initializeBlockchainNetwork(const LogosSvcSettings& settings);


            bool initializeLogging(const LogosSvcSettings& settings);     // Logging
            bool initializeDatabase(const LogosSvcSettings& settings);      // SQL database
            bool initializeDatastorage(const LogosSvcSettings& settings);     // File storage IO
            bool initializeMessaging(const LogosSvcSettings& settings);     // Event brokers


            // Kafka Service
            RdKafka::Conf * _kafka_conf = nullptr;
            RdKafka::Producer * _producer = nullptr;
            RdKafka::Metadata * _brokerMetadata = nullptr;
            EventNotificationDeliveryServiceCb _endServiceCb;
            
            LogosSvcSettings _settings;
            std::unique_ptr<grpc::Server> _serverInstance = nullptr;
            std::shared_ptr<grpc::ServerCredentials> _credentials;
            std::shared_ptr<soci::session> _sqlSession;

            std::vector<std::shared_ptr<spdlog::logger>> _loggers;
            bool _initialized = false;

    };

    // Used for secure computation
    class BlockchainNodeRuntime: public iblockchain_node_runtime
    {
        public:


        private:



    };


}}}