
#include "core/rpc/services/identity_service.h"
#include "core/db/database_manager.h"
#include "core/rpc/utils.h"
#include "string_helpers.h"
#include "macros.h"

namespace acl { namespace logos { namespace core { namespace rpc {


    grpc::Status IdentityService::CreateIdentity(grpc::ServerContext * context, const acl::rpc::e2ee::v1::CreateIdentityRequest * request, acl::rpc::e2ee::v1::CreateIdentityResponse * response)
    {
        BCService()->LogMessage(cpp::utils::stringFormat("Service request: %s::%s", __CLASS_NAME_CSTR__, __METHOD_NAME_CSTR__));
        auto dbInstance = (acl::logos::core::db::DatabaseManager*)(BCService()->DatabaseManager());

        try {
            // Ensure the device exists before attaching an identity.
            const auto device = dbInstance->Devices().Retrieve(request->device_id().value());

            // Ensure the device does not already have an identity.
            // For now, assume Retrieve throws if it exists.
            try
            {
                dbInstance->Identities().Retrieve(request->device_id().value());
                return grpc::Status(grpc::StatusCode::ALREADY_EXISTS, "Device already has an identity");
            }
            catch (...)
            {
                // Identity does not exist, continue.

            }

            acl::logos::core::db::identity identity;
            identity.device_id = request->device_id().value();
            identity.algorithm = request->public_key().algorithm();
            identity.public_key.assign(request->public_key().key().begin(), request->public_key().key().end());
            identity.created_at = std::chrono::system_clock::now();

            auto insertId = dbInstance->Identities().Insert(identity);
            if(insertId > 0) {
                auto* proto_identity = response->mutable_identity();

                proto_identity->mutable_id()->set_value(identity.id);
                proto_identity->mutable_device_id()->set_value(identity.device_id);
                proto_identity->mutable_public_key()->set_algorithm( static_cast<acl::rpc::e2ee::v1::KeyAlgorithm>(identity.algorithm) );
                proto_identity->mutable_public_key()->set_key(identity.public_key);
                *proto_identity->mutable_created_at() = ToProtoTimestamp(identity.created_at);
            }
            return grpc::Status::OK;
        }
        catch (const std::exception& ex)
        {
            BCService()->LogMessage(cpp::utils::stringFormat("Error processing RegisterIdentity request: %s", ex.what()), spdlog::level::err);
            return grpc::Status(grpc::StatusCode::INTERNAL, ex.what());
        }
        return grpc::Status::OK;
    }



    grpc::Status IdentityService::GetIdentity(grpc::ServerContext * context, const acl::rpc::e2ee::v1::GetIdentityRequest * request, acl::rpc::e2ee::v1::GetIdentityResponse * response)
    {
        BCService()->LogMessage(cpp::utils::stringFormat("Service request: %s::%s", __CLASS_NAME_CSTR__, __METHOD_NAME_CSTR__));
        auto dbInstance = (acl::logos::core::db::DatabaseManager*)(BCService()->DatabaseManager());

        try {
            const auto identity = dbInstance->Identities().Retrieve(request->identity_id().value());
            auto* proto_identity = response->mutable_identity();

            proto_identity->mutable_id()->set_value(identity.id);
            proto_identity->mutable_device_id()->set_value(identity.device_id);
            proto_identity->mutable_public_key()->set_algorithm( static_cast<acl::rpc::e2ee::v1::KeyAlgorithm>(identity.algorithm) );
            proto_identity->mutable_public_key()->set_key(identity.public_key);
            *proto_identity->mutable_created_at() = ToProtoTimestamp(identity.created_at);
            return grpc::Status::OK;
        }
        catch (const std::exception& ex)
        {
            BCService()->LogMessage(cpp::utils::stringFormat("Error processing GetIdentity request: %s", ex.what()), spdlog::level::err);
            return grpc::Status(grpc::StatusCode::INTERNAL, ex.what());
        }

        return grpc::Status::OK;
    }


    grpc::Status IdentityService::ListIdentities(grpc::ServerContext * context, const acl::rpc::e2ee::v1::ListIdentitiesRequest * request, acl::rpc::e2ee::v1::ListIdentitiesResponse * response)
    {
        BCService()->LogMessage(cpp::utils::stringFormat("Service request: %s::%s", __CLASS_NAME_CSTR__, __METHOD_NAME_CSTR__));
        auto dbInstance = (acl::logos::core::db::DatabaseManager*)(BCService()->DatabaseManager());

        try
        {
            auto identities = dbInstance->Identities().RetrieveAll();

            for (const auto& identity : identities)
            {
                if (identity.device_id != request->device_id().value())
                    continue;

                if (!request->include_revoked())
                    continue;

                auto* proto = response->add_identities();

                proto->mutable_id()->set_value(0);
                proto->mutable_device_id()->set_value(identity.device_id);
                proto->mutable_public_key()->set_algorithm( static_cast<acl::rpc::e2ee::v1::KeyAlgorithm>(identity.algorithm) );
                proto->mutable_public_key()->set_key(identity.public_key.data(), identity.public_key.size());
                *proto->mutable_created_at() = ToProtoTimestamp(identity.created_at);
                //proto->set_active(identity.active);
            }

            return grpc::Status::OK;
        }
        catch (const std::exception& ex)
        {

            BCService()->LogMessage(cpp::utils::stringFormat("Error processing ListIdentities request: %s", ex.what()), spdlog::level::err);
            return grpc::Status(grpc::StatusCode::INTERNAL, ex.what());

            //auto* error = response->add_errors();

            //error->set_code("INTERNAL_ERROR");
            //error->set_message(ex.what());

            return grpc::Status::OK;
        }


        return grpc::Status::OK;
    }





} } } }