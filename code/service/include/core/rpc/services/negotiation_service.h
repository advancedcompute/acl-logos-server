#pragma once

#include <grpc/grpc.h>
#include <string>

#include "core/config/cmd_arg_options.h"
#include "core/interfaces/blockchain_node.h"
#include "core/rpc/generated/negotiation.grpc.pb.h"


namespace acl { namespace logos { namespace core { namespace rpc {

    class NegotiationService final:
        public acl::logos::core::iblockchain_node_service<LogosSvcSettings>,
        public acl::rpc::e2ee::v1::NegotiationService::Service
    {
    public:
        explicit NegotiationService(iblockchain_node<LogosSvcSettings> * nodePtr):
            acl::rpc::e2ee::v1::NegotiationService::Service(),
            iblockchain_node_service(nodePtr, this)
        {

        }
        ~NegotiationService() override {}


    };

} } } }