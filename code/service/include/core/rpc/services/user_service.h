#pragma once

#include <grpc/grpc.h>
#include <string>

#include "core/config/cmd_arg_options.h"
#include "core/interfaces/blockchain_node.h"
#include "core/rpc/generated/user.grpc.pb.h"


namespace acl { namespace logos { namespace core { namespace rpc {

    class UserService final:
        public acl::logos::core::iblockchain_node_service<LogosSvcSettings>,
        public acl::rpc::e2ee::v1::UserService::Service
    {
    public:
        explicit UserService(iblockchain_node<LogosSvcSettings> * nodePtr):
            acl::rpc::e2ee::v1::UserService::Service(),
            iblockchain_node_service(nodePtr, this)
        {

        }
        ~UserService() override {}

        grpc::Status CreateUser(grpc::ServerContext * context, const acl::rpc::e2ee::v1::CreateUserRequest * request, acl::rpc::e2ee::v1::CreateUserResponse * response) override;
        grpc::Status GetUser(grpc::ServerContext * context, const acl::rpc::e2ee::v1::GetUserRequest * request, acl::rpc::e2ee::v1::GetUserResponse * response) override;
        grpc::Status UpdateUser(grpc::ServerContext * context, const acl::rpc::e2ee::v1::UpdateUserRequest * request, acl::rpc::e2ee::v1::UpdateUserResponse * response) override;
        grpc::Status DeleteUser(grpc::ServerContext * context, const acl::rpc::e2ee::v1::DeleteUserRequest * request, google::protobuf::Empty * response) override;
        grpc::Status ListUsers(grpc::ServerContext * context, const acl::rpc::e2ee::v1::ListUsersRequest * request, acl::rpc::e2ee::v1::ListUsersResponse * response) override;

    private:

        bool validateCreateUserRequest(const acl::rpc::e2ee::v1::CreateUserRequest * request);
        bool validateUpdateUserRequest(const acl::rpc::e2ee::v1::UpdateUserRequest * request);
        
    };

} } } }