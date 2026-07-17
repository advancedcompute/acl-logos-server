
#include "core/rpc/services/identity_service.h"
#include "core/db/database_manager.h"
#include "string_helpers.h"
#include "macros.h"

namespace acl { namespace logos { namespace core { namespace rpc {

    grpc::Status IdentityService::RegisterIdentity(grpc::ServerContext * context, const acl::rpc::e2ee::v1::RegisterIdentityRequest * request, acl::rpc::e2ee::v1::RegisterIdentityResponse * response)
    {
        BCService()->LogMessage(cpp::utils::stringFormat("Service request: %s::%s", __CLASS_NAME_CSTR__, __METHOD_NAME_CSTR__));

        auto dbInstance = (acl::logos::core::db::DatabaseManager*)(BCService()->DatabaseManager());



        return grpc::Status::OK;
    }


    grpc::Status IdentityService::GetIdentity(grpc::ServerContext * context, const acl::rpc::e2ee::v1::GetIdentityRequest * request, acl::rpc::e2ee::v1::GetIdentityResponse * response)
    {
        BCService()->LogMessage(cpp::utils::stringFormat("Service request: %s::%s", __CLASS_NAME_CSTR__, __METHOD_NAME_CSTR__));

        auto dbInstance = (acl::logos::core::db::DatabaseManager*)(BCService()->DatabaseManager());


        return grpc::Status::OK;
    }

} } } }