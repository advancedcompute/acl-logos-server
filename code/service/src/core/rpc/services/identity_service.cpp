
#include "core/rpc/services/identity_service.h"

namespace acl { namespace logos { namespace core { namespace rpc {

grpc::Status IdentityService::RegisterIdentity(grpc::ServerContext * context, const acl::rpc::v1::RegisterIdentityRequest * request, acl::rpc::v1::RegisterIdentityResponse * response)
{
    if(request->first_name().empty() || request->last_name().empty())
    {
        auto errObj = response->add_errors();
        errObj->set_code(0);
        errObj->set_detail("Identity must include first & last name");
        return grpc::Status::OK;
    }

    if(request->public_key().empty())
    {
        auto errObj = response->add_errors();
        errObj->set_code(0);
        errObj->set_detail("Identity must a public key");
        return grpc::Status::OK;
    } else {
        // Validate key is either ECC, ED25519 or RSA
    }

    if(!request->metadata().empty())
    {
        // Validate metadata e.g. GPS-coordinates at time of registration,
        // biometrics attached to ID etc.
    }

    auto dobTs = request->dob();
    

    // Add to database



    return grpc::Status::OK;
}

} } } }