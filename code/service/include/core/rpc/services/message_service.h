#pragma once

#include <grpc/grpc.h>
#include <string>

#include "core/config/cmd_arg_options.h"
#include "core/interfaces/blockchain_node.h"
#include "core/rpc/generated/message.grpc.pb.h"


namespace acl { namespace logos { namespace core { namespace rpc {

    class MessagingService final:
        public acl::logos::core::iblockchain_node_service<LogosSvcSettings>,
        public acl::rpc::e2ee::v1::MessagingService::Service
    {
    public:
        explicit MessagingService(iblockchain_node<LogosSvcSettings> * nodePtr):
            acl::rpc::e2ee::v1::MessagingService::Service(),
            iblockchain_node_service(nodePtr, this)
        {

        }
        ~MessagingService() override {}


    };

} } } }