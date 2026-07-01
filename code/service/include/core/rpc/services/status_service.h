#pragma once

#include <grpc/grpc.h>
#include <string>

#include "core/interfaces/blockchain_node.h"
#include "core/rpc/generated/status.grpc.pb.h"

using grpc::ServerContext;
using google::protobuf::Empty;

namespace acl { namespace logos { namespace core { namespace rpc {

    class StatusService final:
        public acl::logos::core::iblockchain_node_service,
        public acl::rpc::Status::Service
    {
    public:
        explicit StatusService():
            acl::rpc::Status::Service(),
            iblockchain_node_service(this)
        {

        }
        ~StatusService() override {}

        grpc::Status Up(ServerContext * context, const Empty * request, acl::rpc::StatusInfo * response) override;

    private:
    };

} } } }