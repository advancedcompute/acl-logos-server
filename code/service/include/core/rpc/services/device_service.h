#pragma once

#include <grpc/grpc.h>
#include <string>

#include "core/config/cmd_arg_options.h"
#include "core/interfaces/blockchain_node.h"
#include "core/rpc/generated/device.grpc.pb.h"

namespace acl { namespace logos { namespace core { namespace rpc {

    class DeviceService final:
        public acl::logos::core::iblockchain_node_service<LogosSvcSettings>,
        public acl::rpc::e2ee::v1::DeviceService::Service
    {
    public:
        explicit DeviceService(iblockchain_node<LogosSvcSettings> * nodePtr):
            acl::rpc::e2ee::v1::DeviceService::Service(),
            iblockchain_node_service(nodePtr, this)
        {

        }
        ~DeviceService() override {}

    private:
    };

} } } }