#pragma once

#include "core/config/cmd_arg_options.h"
#include "core/interfaces/blockchain_node.h"

#include "spdlog/logger.h"

#include <grpc++/server.h>
//#include <grpc++/server_builder.h>
#include <grpc++/security/server_credentials.h>
#include <librdkafka/rdkafkacpp.h>

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
            virtual ~BlockchainNode() = default;

            
            bool Initialize(const LogosSvcSettings& settings);
            void Run();
            void Shutdown();

            const std::vector<std::shared_ptr<spdlog::logger>> Loggers() { return _loggers; }




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