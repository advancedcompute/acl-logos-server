#pragma once

#include <grpc/grpc.h>
#include <string>

#include "core/config/cmd_arg_options.h"
#include "core/interfaces/blockchain_node.h"
#include "core/rpc/generated/identity.grpc.pb.h"


namespace acl { namespace logos { namespace core { namespace rpc {

    class IdentityService final:
        public acl::logos::core::iblockchain_node_service<LogosSvcSettings>,
        public acl::rpc::v1::IdentityService::Service
    {
    public:
        explicit IdentityService(iblockchain_node<LogosSvcSettings> * nodePtr):
            acl::rpc::v1::IdentityService::Service(),
            iblockchain_node_service(nodePtr, this)
        {

        }
        ~IdentityService() override {}

        grpc::Status RegisterIdentity(grpc::ServerContext * context, const acl::rpc::v1::RegisterIdentityRequest * request, acl::rpc::v1::RegisterIdentityResponse * response) override;

    private:

        bool validate(const acl::rpc::v1::RegisterIdentityRequest * request);

    };

} } } }