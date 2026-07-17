
#include "core/rpc/services/user_service.h"
#include "core/rpc/utils.h"
#include "core/db/database_manager.h"
#include "string_helpers.h"
#include "uuid.h"
#include "macros.h"

namespace acl { namespace logos { namespace core { namespace rpc {

    grpc::Status UserService::CreateUser(grpc::ServerContext * context, const acl::rpc::e2ee::v1::CreateUserRequest * request, acl::rpc::e2ee::v1::CreateUserResponse * response)
    {
        BCService()->LogMessage(cpp::utils::stringFormat("Service request: %s::%s", __CLASS_NAME_CSTR__, __METHOD_NAME_CSTR__));
        auto dbInstance = (acl::logos::core::db::DatabaseManager*)(BCService()->DatabaseManager());

        try
        {
            acl::logos::core::db::user new_user;
            
            new_user.first_name = request->first_name();
            new_user.last_name = request->last_name();
            new_user.display_name = request->display_name();
            new_user.phone_number = request->phone_number();
            new_user.email_address = request->email_address();

            new_user.email_verified = false;
            new_user.phone_verified = false;
            new_user.created_at = std::chrono::system_clock::now();

            dbInstance->Users().Insert(new_user);
            auto* user = response->mutable_user();

            user->set_id(new_user.id);
            user->set_first_name(new_user.first_name);
            user->set_last_name(new_user.last_name);
            user->set_display_name(new_user.display_name);
            user->set_phone_number(new_user.phone_number);
            user->set_email_address(new_user.email_address);
            user->set_email_verified(new_user.email_verified);
            user->set_phone_verified(new_user.phone_verified);

            *user->mutable_created_at() = ToProtoTimestamp(new_user.created_at);
            return grpc::Status::OK;
        }
        catch (const std::exception& ex)
        {
            BCService()->LogMessage(cpp::utils::stringFormat("Error processing CreateUser request: %s", ex.what()), spdlog::level::err);
            return grpc::Status(grpc::StatusCode::INTERNAL, ex.what());
        }        
        return grpc::Status::OK;
    }
    
    grpc::Status UserService::GetUser(grpc::ServerContext * context, const acl::rpc::e2ee::v1::GetUserRequest * request, acl::rpc::e2ee::v1::GetUserResponse * response)
    {
        BCService()->LogMessage(cpp::utils::stringFormat("Service request: %s::%s", __CLASS_NAME_CSTR__, __METHOD_NAME_CSTR__));
        auto dbInstance = (acl::logos::core::db::DatabaseManager*)(BCService()->DatabaseManager());

        db::user user;
        try {
            user = dbInstance->Users().Retrieve(request->user_id());
        } catch(std::runtime_error& ex)
        {
            BCService()->LogMessage(cpp::utils::stringFormat("Error processing GetUser request: %s", ex.what()), spdlog::level::err);
            return grpc::Status(grpc::StatusCode::NOT_FOUND, "User not found");
        }

        try
        {
            auto* proto_user = response->mutable_user();

            proto_user->set_id(user.id);
            proto_user->set_first_name(user.first_name);
            proto_user->set_last_name(user.last_name);
            proto_user->set_display_name(user.display_name);
            proto_user->set_phone_number(user.phone_number);
            proto_user->set_email_address(user.email_address);
            proto_user->set_email_verified(user.email_verified);
            proto_user->set_phone_verified(user.phone_verified);

            *proto_user->mutable_created_at() = ToProtoTimestamp(user.created_at);
            return grpc::Status::OK;
        }
        catch (const std::exception& ex)
        {
            BCService()->LogMessage(cpp::utils::stringFormat("Error processing GetUser request: %s", ex.what()), spdlog::level::err);
            return grpc::Status(grpc::StatusCode::INTERNAL, ex.what());
        }
        return grpc::Status::OK;
    }
    
    grpc::Status UserService::UpdateUser(grpc::ServerContext * context, const acl::rpc::e2ee::v1::UpdateUserRequest * request, acl::rpc::e2ee::v1::UpdateUserResponse * response)
    {
        BCService()->LogMessage(cpp::utils::stringFormat("Service request: %s::%s", __CLASS_NAME_CSTR__, __METHOD_NAME_CSTR__));
        auto dbInstance = (acl::logos::core::db::DatabaseManager*)(BCService()->DatabaseManager());

        db::user user;
        try {
            user = dbInstance->Users().Retrieve(request->user_id());
        } catch(std::runtime_error& ex)
        {
            BCService()->LogMessage(cpp::utils::stringFormat("Error processing UpdateUser request: %s", ex.what()), spdlog::level::err);
            return grpc::Status(grpc::StatusCode::NOT_FOUND, "User not found");
        }
        
        try
        {
            if (request->has_first_name())
                user.first_name = request->first_name();

            if (request->has_last_name())
                user.last_name = request->last_name();

            if (request->has_display_name())
                user.display_name = request->display_name();

            if (request->has_phone_number())
                user.phone_number = request->phone_number();

            if (request->has_email_address())
                user.email_address = request->email_address();
            
            dbInstance->Users().Update(user);

            auto* proto_user = response->mutable_user();
            proto_user->set_id(user.id);
            proto_user->set_first_name(user.first_name);
            proto_user->set_last_name(user.last_name);
            proto_user->set_display_name(user.display_name);
            proto_user->set_phone_number(user.phone_number);
            proto_user->set_email_address(user.email_address);
            proto_user->set_email_verified(user.email_verified);
            proto_user->set_phone_verified(user.phone_verified);

            *proto_user->mutable_created_at() = ToProtoTimestamp(user.created_at);
            return grpc::Status::OK;
        }
        catch (const std::exception& ex)
        {
            BCService()->LogMessage(cpp::utils::stringFormat("Error processing UpdateUser request: %s", ex.what()), spdlog::level::err);
            return grpc::Status(grpc::StatusCode::INTERNAL, ex.what());
        }

        return grpc::Status::OK;
    }
    
    grpc::Status UserService::DeleteUser(grpc::ServerContext * context, const acl::rpc::e2ee::v1::DeleteUserRequest * request, google::protobuf::Empty * response)
    {
        BCService()->LogMessage(cpp::utils::stringFormat("Service request: %s::%s", __CLASS_NAME_CSTR__, __METHOD_NAME_CSTR__));
        auto dbInstance = (acl::logos::core::db::DatabaseManager*)(BCService()->DatabaseManager());

        // TODO //
        // Note: It may be easier to implement a soft delete. Gives us ability to offer an 'undo' if a user changes their mind etc.

        return grpc::Status::OK;
    }
    
    grpc::Status UserService::ListUsers(grpc::ServerContext * context, const acl::rpc::e2ee::v1::ListUsersRequest * request, acl::rpc::e2ee::v1::ListUsersResponse * response)
    {
        BCService()->LogMessage(cpp::utils::stringFormat("Service request: %s::%s", __CLASS_NAME_CSTR__, __METHOD_NAME_CSTR__));
        auto dbInstance = (acl::logos::core::db::DatabaseManager*)(BCService()->DatabaseManager());

        try
        {
            const auto users = dbInstance->Users().RetrieveAll();
            for (const auto& user : users)
            {
                auto* proto_user = response->add_users();
                proto_user->set_id(user.id);
                proto_user->set_first_name(user.first_name);
                proto_user->set_last_name(user.last_name);
                proto_user->set_display_name(user.display_name);
                proto_user->set_phone_number(user.phone_number);
                proto_user->set_email_address(user.email_address);
                proto_user->set_email_verified(user.email_verified);
                proto_user->set_phone_verified(user.phone_verified);

                *proto_user->mutable_created_at() = ToProtoTimestamp(user.created_at);
            }

            // Pagination can be implemented later.
            response->set_next_page_token("");
            return grpc::Status::OK;
        }
        catch (const std::exception& ex)
        {
            BCService()->LogMessage(cpp::utils::stringFormat("Error processing UpdateUser request: %s", ex.what()), spdlog::level::err);
            return grpc::Status(grpc::StatusCode::INTERNAL, ex.what());
        }
        return grpc::Status::OK;
    }

} } } }