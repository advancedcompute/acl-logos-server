#pragma once

#include <memory>
#include <chrono>
#include <type_traits>
#include <grpc++/grpc++.h>
#include "soci/soci.h"

#include <spdlog/spdlog.h>
#include <vector>

namespace acl { namespace logos { namespace core {

    template <class SettingsObject>
    class iblockchain_node
    {
        public:

            std::shared_ptr<soci::session>& SQLSession() { return _sql_session; }

            iblockchain_node() {
                _start_time = std::chrono::system_clock::now();
            }

            std::chrono::system_clock::time_point start_time() { return _start_time; }

            std::vector<std::shared_ptr<spdlog::logger>>& Loggers() { return _loggers; }

            SettingsObject& Settings() { return _settings; }


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
            std::shared_ptr<soci::session> _sql_session = nullptr;
            std::chrono::system_clock::time_point _start_time;
            std::vector<std::shared_ptr<spdlog::logger>> _loggers;
            SettingsObject  _settings;
    };



    template <class SettingsObject>
    class iblockchain_node_service
    {
        public:
            iblockchain_node_service(iblockchain_node<SettingsObject> * nodePtr, grpc::Service * service = nullptr): _node(nodePtr), _service(service)
            {
                assert(nodePtr != nullptr);
            }
            
            grpc::Service * RPCService() { return _service; }

            iblockchain_node<SettingsObject> * BCService() { return _node; }

        private:
            grpc::Service * _service = nullptr;
            iblockchain_node<SettingsObject> * _node = nullptr;
    };


}
}
}