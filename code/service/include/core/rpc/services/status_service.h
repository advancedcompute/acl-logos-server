#pragma once

#include <grpc/grpc.h>
#include <string>

#include "core/config/cmd_arg_options.h"
#include "core/interfaces/blockchain_node.h"
#include "core/rpc/generated/status.grpc.pb.h"


namespace acl { namespace logos { namespace core { namespace rpc {

    class StatusService final:
        public acl::logos::core::iblockchain_node_service<LogosSvcSettings>,
        public acl::rpc::e2ee::v1::Status::Service
    {
    public:
        explicit StatusService(iblockchain_node<LogosSvcSettings> * nodePtr):
            acl::rpc::e2ee::v1::Status::Service(),
            iblockchain_node_service(nodePtr, this)
        {

        }
        ~StatusService() override {}

        grpc::Status Up(grpc::ServerContext * context, const google::protobuf::Empty * request, acl::rpc::e2ee::v1::StatusInfo * response) override;

    private:
    };

} } } }