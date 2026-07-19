
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
            /*
            // Ensure the device exists before attaching an identity.
            const auto device = dbInstance->Devices().Retrieve(request->device_id());

            // Ensure the device does not already have an identity.
            //
            // Depending on your EntityManager design, this would ideally
            // be a specific Exists/RetrieveByDeviceId check.
            //
            // For now, assume Retrieve throws if it exists.
            try
            {
                EntityManager<acl::logos::core::db::identity>::Retrieve(
                    m_database,
                    request->device_id());

                return grpc::Status(
                    grpc::StatusCode::ALREADY_EXISTS,
                    "Device already has an identity");
            }
            catch (...)
            {
                // Identity does not exist, continue.
            }


            acl::logos::core::db::identity identity;

            identity.device_id = request->device_id();
            identity.algorithm = request->algorithm();
            identity.public_key.assign(request->public_key().begin(), request->public_key().end());
            identity.created_at = std::chrono::system_clock::now();

            EntityManager<acl::logos::core::db::identity>::Insert(
                m_database,
                identity);

            auto* proto_identity = response->mutable_identity();

            proto_identity->set_device_id(identity.device_id);
            proto_identity->set_algorithm(identity.algorithm);
            proto_identity->set_public_key(identity.public_key.data(), identity.public_key.size());
            *proto_identity->mutable_created_at() = ToProtoTimestamp(identity.created_at);
            */
            
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
            /*
            const auto identity = dbInstance->Identities().Retrieve(request->device_id());

            auto* proto_identity = response->mutable_identity();

            proto_identity->set_device_id(identity.device_id);
            proto_identity->set_algorithm(identity.algorithm);
            proto_identity->set_public_key(identity.public_key.data(), identity.public_key.size());
            *proto_identity->mutable_created_at() = ToProtoTimestamp(identity.created_at);
            */
            
            return grpc::Status::OK;
        }
        catch (const std::exception& ex)
        {
            BCService()->LogMessage(cpp::utils::stringFormat("Error processing GetIdentity request: %s", ex.what()), spdlog::level::err);
            return grpc::Status(grpc::StatusCode::INTERNAL, ex.what());
        }

        return grpc::Status::OK;
    }

} } } }