
#include "core/rpc/services/identity_service.h"
#include "core/db/database_manager.h"
#include <chrono>
#include <sstream>

#include "uuid.h"
#include "ecc.h"
#include "ed25519.h"
#include "rsa.h"


namespace acl { namespace logos { namespace core { namespace rpc {

    bool IdentityService::validate_identity_register_request(const acl::rpc::v1::RegisterIdentityRequest * request)
    {
        if(request->public_key().empty()) return false;

        cpp::utils::ED25519 ed; cpp::utils::ECC ecc;
        if(!ed.set_public_key_pem(request->public_key()) ||
            !ecc.load_own_public_key_from_pem(request->public_key())) return false;
        
        return true;
    }
    
    // TODO: Move elsewhere, generalise
    void type_converter(db::IdentityType id_type, acl::rpc::v1::IdentityType& rpc_id_type)
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
    
    void IdentityService::prepare_identity_register_response(const db::Identity& identity, acl::rpc::v1::RegisterIdentityResponse * response)
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

    void IdentityService::prepare_identities_list_response(const std::vector<db::Identity>& ids, acl::rpc::v1::ListIdentitiesResponse * response)
    {
        // TODO: Add paging support
        for(auto identity : ids)
        {
            auto responseId = response->add_identities();
            responseId->set_identity_id(identity.identity_id);
            responseId->set_public_key(identity.public_key);
            responseId->set_active(identity.active);
            responseId->set_verified(identity.verified);

            acl::rpc::v1::IdentityType responseType;
            type_converter(identity.identity_type, responseType);
            responseId->set_type(responseType);

            auto jsonMetadata = db::TableTraits<db::Identity>::JsonMetadata(identity.metadata);
            for(const std::string& member : jsonMetadata.getMemberNames())
            {
                printf("member: %s\n", member.c_str());
                (*responseId->mutable_metadata())[member] = jsonMetadata[member].asString();
            }
            responseId->mutable_created_ts()->set_seconds(identity.created_at);
            
        }
    }

    grpc::Status IdentityService::RegisterIdentity(grpc::ServerContext * context, const acl::rpc::v1::RegisterIdentityRequest * request, acl::rpc::v1::RegisterIdentityResponse * response)
    {
        BCService()->LogMessage(cpp::utils::stringFormat("Service request: %s::%s", __CLASS_NAME_CSTR__, __METHOD_NAME_CSTR__));

        try {
            if(validate_identity_register_request(request))
            {
                db::Identity identity;
                identity.identity_id = cpp::utils::generate_uuid();
                identity.public_key = request->public_key();
                identity.identity_type = (db::IdentityType)request->type();
                identity.active = true;
                identity.verified = false;

                auto currentTime = std::chrono::system_clock::now().time_since_epoch();
                auto currentTimeS = std::chrono::duration_cast<std::chrono::seconds>(currentTime);
                identity.created_at = currentTimeS.count();
                
                for(auto& kv : request->metadata()) {
                    identity.metadata[std::string(kv.first)] = std::string(kv.second);
                }

                // Add to database
                db::DatabaseManager* dbManager = (db::DatabaseManager*)(BCService()->DatabaseManager());
                if(dbManager->Identities().Insert(identity))
                {
                    BCService()->LogMessage(cpp::utils::stringFormat("Created new identity %s", identity.identity_id));
                    prepare_identity_register_response(identity, response);
                } else {
                    BCService()->LogMessage("Identity registration request failed: Database insert failed");
                    auto errObj = response->add_errors();
                    errObj->set_code(0);
                    errObj->set_detail("Identity registration request failed");
                }
            } else {
                auto errObj = response->add_errors();
                errObj->set_code(0);
                errObj->set_detail("Identity registration request failed validation");
            }
        } catch(std::exception& ex)
        {
            BCService()->LogMessage(cpp::utils::stringFormat("Exception when handling user request: %s", ex.what()), spdlog::level::err);
            auto errObj = response->add_errors();
            errObj->set_code(0);    // TODO
            errObj->set_detail("Exception when handling request");
        } catch(...)
        {
            BCService()->LogMessage("Exception when handling request", spdlog::level::err);
            auto errObj = response->add_errors();
            errObj->set_code(0);
            errObj->set_detail("Exception when handling request");
        }

        return grpc::Status::OK;
    }

    grpc::Status IdentityService::ListIdentities(grpc::ServerContext * context, const acl::rpc::v1::ListIdentitiesRequest * request, acl::rpc::v1::ListIdentitiesResponse * response)
    {
        try {

            BCService()->LogMessage(cpp::utils::stringFormat("Service request: %s::%s", __CLASS_NAME_CSTR__, __METHOD_NAME_CSTR__));

            db::DatabaseManager* dbManager = (db::DatabaseManager*)(BCService()->DatabaseManager());
            auto storedIdentities = dbManager->Identities().RetrieveAll();
            prepare_identities_list_response(storedIdentities, response);
        } catch(std::exception& ex)
        {
            BCService()->LogMessage(cpp::utils::stringFormat("Exception when handling user request: %s", ex.what()), spdlog::level::err);
        } catch(...)
        {
            BCService()->LogMessage("Exception when handling request", spdlog::level::err);
        }
        return grpc::Status::OK;
    }


} } } }