#pragma once

#include <grpc/grpc.h>
#include <string>

#include "core/config/cmd_arg_options.h"
#include "core/interfaces/blockchain_node.h"
#include "core/rpc/generated/identity.grpc.pb.h"

using grpc::ServerContext;
using google::protobuf::Empty;

namespace acl { namespace logos { namespace core { namespace rpc {

    class IdentityService final:
        public acl::logos::core::iblockchain_node_service<LogosSvcSettings>,
        public acl::rpc::Identity::Service
    {
    public:
        explicit IdentityService(iblockchain_node<LogosSvcSettings> * nodePtr):
            acl::rpc::Identity::Service(),
            iblockchain_node_service(nodePtr, this)
        {

        }
        ~IdentityService() override {}

        grpc::Status Issue(ServerContext * context, const acl::rpc::IdentityIssueRequest * request, acl::rpc::IdentityIssueResponse * response) override;

    private:
    };

} } } }