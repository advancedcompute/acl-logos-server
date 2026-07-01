#pragma once

#include "core/config/cmd_arg_options.h"
#include "core/interfaces/blockchain_node.h"

#include <grpc++/server.h>
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


    class BlockchainNode: public iblockchain_node<LogosSvcSettings>
    {
        public:
            BlockchainNode(): iblockchain_node<LogosSvcSettings>() {}
            virtual ~BlockchainNode() {
                Shutdown();
            }

            
            bool Initialize(const LogosSvcSettings& settings);
            void Run();

            std::shared_ptr<grpc::Server>& GRPCServer() { return _serverInstance; }


        private:
            bool initializeGRPCServer(const LogosSvcSettings& settings);
            bool initializeGRPCServices(const LogosSvcSettings& settings);

            //bool initializeBlockchainIdentity(const LogosSvcSettings& settings);
            //bool initializeBlockchainNetwork(const LogosSvcSettings& settings);


            bool initializeLogging(const LogosSvcSettings& settings);     // Logging
            bool initializeDatabase(const LogosSvcSettings& settings);      // SQL database
            bool initializeDatastorage(const LogosSvcSettings& settings);     // File storage IO
            bool initializeMessaging(const LogosSvcSettings& settings);     // Event brokers
            
            void Shutdown();

            // Kafka Service
            RdKafka::Conf * _kafka_conf = nullptr;
            RdKafka::Producer * _producer = nullptr;
            RdKafka::Metadata * _brokerMetadata = nullptr;
            EventNotificationDeliveryServiceCb _endServiceCb;
            
            // GRPC
            std::vector<std::shared_ptr<iblockchain_node_service<LogosSvcSettings>>> _nodeServiceVect;
            std::map<std::string, std::shared_ptr<iblockchain_node_service<LogosSvcSettings>>> _nodeServiceMap;
            std::shared_ptr<grpc::Server> _serverInstance = nullptr;
            std::shared_ptr<grpc::ServerCredentials> _credentials;
            std::thread _grpcServerThread;
            
            bool _initialized = false;

    };


}}}