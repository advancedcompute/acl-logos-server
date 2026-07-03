#pragma once

#include <grpc/grpc.h>
#include <string>

#include "core/config/cmd_arg_options.h"
#include "core/db/models/identity.h"
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

        grpc::Status ListIdentities(grpc::ServerContext * context, const acl::rpc::v1::ListIdentitiesRequest * request, acl::rpc::v1::ListIdentitiesResponse * response) override;

    private:

        bool validate_identity_register_request(const acl::rpc::v1::RegisterIdentityRequest * request);

        void prepare_identity_register_response(const db::Identity& identity, acl::rpc::v1::RegisterIdentityResponse * response);
        void prepare_identities_list_response(const std::vector<db::Identity>& ids, acl::rpc::v1::ListIdentitiesResponse * response);

    };

} } } }