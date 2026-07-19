#pragma once

#include <grpc/grpc.h>
#include <string>

#include "core/config/cmd_arg_options.h"
#include "core/interfaces/blockchain_node.h"
#include "core/rpc/generated/identity.grpc.pb.h"


namespace acl { namespace logos { namespace core { namespace rpc {

    class IdentityService final:
        public acl::logos::core::iblockchain_node_service<LogosSvcSettings>,
        public acl::rpc::e2ee::v1::IdentityService::Service
    {
    public:
        explicit IdentityService(iblockchain_node<LogosSvcSettings> * nodePtr):
            acl::rpc::e2ee::v1::IdentityService::Service(),
            iblockchain_node_service(nodePtr, this)
        {
            
        }
        ~IdentityService() override {}

        grpc::Status CreateIdentity(grpc::ServerContext * context, const acl::rpc::e2ee::v1::CreateIdentityRequest * request, acl::rpc::e2ee::v1::CreateIdentityResponse * response) override;

        grpc::Status GetIdentity(grpc::ServerContext * context, const acl::rpc::e2ee::v1::GetIdentityRequest * request, acl::rpc::e2ee::v1::GetIdentityResponse * response) override;

    private:
    };

} } } }