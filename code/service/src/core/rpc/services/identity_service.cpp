
#include "core/rpc/services/identity_service.h"
#include "core/db/database_manager.h"
#include <chrono>
#include <sstream>

#include "uuid.h"
#include "ecc.h"
#include "ed25519.h"
#include "rsa.h"


namespace acl { namespace logos { namespace core { namespace rpc {

    bool IdentityService::validate(const acl::rpc::v1::RegisterIdentityRequest * request)
    {
        if(request->public_key().empty()) return false;

        cpp::utils::ED25519 ed; cpp::utils::ECC ecc;
        if(!ed.set_public_key_pem(request->public_key()) ||
            !ecc.load_own_public_key_from_pem(request->public_key())) return false;
        
        return true;
    }

    void converter(db::IdentityType id_type, acl::rpc::v1::IdentityType& rpc_id_type)
    {
        switch(id_type)
        {
            case db::IdentityType::PERSON:
                rpc_id_type = acl::rpc::v1::IdentityType::PERSON; break;
            case db::IdentityType::COMPANY:
                rpc_id_type = acl::rpc::v1::IdentityType::COMPANY; break;
            case db::IdentityType::VALIDATOR:
                rpc_id_type = acl::rpc::v1::IdentityType::VALIDATOR; break;
            case db::IdentityType::SERVICE:
                rpc_id_type = acl::rpc::v1::IdentityType::SERVICE; break;
            case db::IdentityType::ORACLE:
                rpc_id_type = acl::rpc::v1::IdentityType::ORACLE; break;
            case db::IdentityType::DAO:
                rpc_id_type = acl::rpc::v1::IdentityType::DAO; break;
        }
    }

    void converter(db::Identity& identity, acl::rpc::v1::RegisterIdentityResponse * response)
    {
        response->mutable_identity()->set_identity_id(identity.identity_id);
        response->mutable_identity()->set_public_key(identity.public_key);
        db::IdentityType idType;
        switch(idType)
        {
            case db::IdentityType::PERSON:
                response->mutable_identity()->set_type(acl::rpc::v1::IdentityType::PERSON); break;
            case db::IdentityType::COMPANY:
                response->mutable_identity()->set_type(acl::rpc::v1::IdentityType::COMPANY); break;
            case db::IdentityType::VALIDATOR:
                response->mutable_identity()->set_type(acl::rpc::v1::IdentityType::VALIDATOR); break;
            case db::IdentityType::SERVICE:
                response->mutable_identity()->set_type(acl::rpc::v1::IdentityType::SERVICE); break;
            case db::IdentityType::ORACLE:
                response->mutable_identity()->set_type(acl::rpc::v1::IdentityType::ORACLE); break;
            case db::IdentityType::DAO:
                response->mutable_identity()->set_type(acl::rpc::v1::IdentityType::DAO); break;
        }

        response->mutable_identity()->set_active(identity.active);
        response->mutable_identity()->set_verified(identity.verified);

        for(auto& kv : identity.metadata) {
            (*response->mutable_identity()->mutable_metadata())[kv.first] = kv.second;
        }

        auto createdTs = new google::protobuf::Timestamp;
        createdTs->set_seconds(identity.created_at);
        response->mutable_identity()->set_allocated_created_ts(createdTs);
    }


grpc::Status IdentityService::RegisterIdentity(grpc::ServerContext * context, const acl::rpc::v1::RegisterIdentityRequest * request, acl::rpc::v1::RegisterIdentityResponse * response)
{
    BCService()->LogMessage(cpp::utils::stringFormat("Service request: %s::%s", __CLASS_NAME_CSTR__, __METHOD_NAME_CSTR__));

    // 1. Validate request
    if(validate(request))
    {
        db::Identity identity;
        identity.identity_id = cpp::utils::generate_uuid();
        identity.public_key = request->public_key();
        identity.identity_type = (db::IdentityType)request->type();
        identity.active = true;
        
        // Crude map copy
        for(auto& kv : request->metadata()) {
            identity.metadata[kv.first] = kv.second;
        }

        // Add to database
        db::DatabaseManager* dbManager = (db::DatabaseManager*)(BCService()->DatabaseManager());
        dbManager->Identities().Insert(identity);

        BCService()->LogMessage(cpp::utils::stringFormat("Created new identitiy %s", identity.identity_id));

        converter(identity, response);
    } else {
        auto errObj = response->add_errors();
        errObj->set_code(0);
        errObj->set_detail("Identity registration request failed validation");
        return grpc::Status::OK;
    }

    return grpc::Status::OK;
}

} } } }