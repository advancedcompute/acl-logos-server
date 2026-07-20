
#include "core/rpc/services/device_service.h"
#include "core/db/database_manager.h"
#include "core/rpc/utils.h"
#include "string_helpers.h"
#include "macros.h"

namespace acl { namespace logos { namespace core { namespace rpc {

    grpc::Status DeviceService::RegisterDevice(grpc::ServerContext * context, const acl::rpc::e2ee::v1::RegisterDeviceRequest * request, acl::rpc::e2ee::v1::RegisterDeviceResponse * response)
    {
        BCService()->LogMessage(cpp::utils::stringFormat("Service request: %s::%s", __CLASS_NAME_CSTR__, __METHOD_NAME_CSTR__));
        auto dbInstance = (acl::logos::core::db::DatabaseManager*)(BCService()->DatabaseManager());

        try
        {
            if(validateRegisterDeviceRequest(request))
            {
                db::user user;
                try {
                    user = dbInstance->Users().Retrieve(request->user_id().value());
                } catch(std::runtime_error& ex)
                {
                    BCService()->LogMessage(cpp::utils::stringFormat("Error processing RegisterDevice request: %s", ex.what()), spdlog::level::err);
                    return grpc::Status(grpc::StatusCode::NOT_FOUND, "User not found");
                }

                acl::logos::core::db::device device;
                device.user_id = request->user_id().value();
                device.device_id = request->client_device_id();
                device.device_name = request->device_name();
                device.type = request->type();
                device.active = true;
                device.created_at = std::chrono::system_clock::now();

                auto insertId = dbInstance->Devices().Insert(device);
                if(insertId > 0) {
                    device.id = insertId;

                    auto* proto_device = response->mutable_device();
                    proto_device->mutable_id()->set_value(device.id);
                    proto_device->set_name(device.device_name);
                    proto_device->set_type( static_cast<acl::rpc::e2ee::v1::DeviceType>(device.type) );
                    proto_device->set_active(device.active);

                    *proto_device->mutable_created_at() = ToProtoTimestamp(device.created_at);
                    return grpc::Status::OK;
                } else {
                    // TODO
                    
                }
            } else {
                // TODO
            }
        }
        catch (const std::exception& ex)
        {
            BCService()->LogMessage(cpp::utils::stringFormat("Error processing RegisterDevice request: %s", ex.what()), spdlog::level::err);
            return grpc::Status(grpc::StatusCode::INTERNAL, ex.what());
        }
    }


    grpc::Status DeviceService::GetDevice(grpc::ServerContext * context, const acl::rpc::e2ee::v1::GetDeviceRequest * request, acl::rpc::e2ee::v1::GetDeviceResponse * response)
    {
        BCService()->LogMessage(cpp::utils::stringFormat("Service request: %s::%s", __CLASS_NAME_CSTR__, __METHOD_NAME_CSTR__));
        auto dbInstance = (acl::logos::core::db::DatabaseManager*)(BCService()->DatabaseManager());

        try
        {
            const auto device = dbInstance->Devices().Retrieve(request->device_id().value());

            auto* proto_device = response->mutable_device();
            proto_device->mutable_id()->set_value(device.id);
            proto_device->set_name( device.device_name );
            proto_device->set_type( static_cast<acl::rpc::e2ee::v1::DeviceType>(device.type) );
            proto_device->set_active(device.active);

            *proto_device->mutable_created_at() = ToProtoTimestamp(device.created_at);
            *proto_device->mutable_last_seen_at() = ToProtoTimestamp(device.last_seen_at);
            return grpc::Status::OK;
        }
        catch (const std::exception& ex)
        {
            BCService()->LogMessage(cpp::utils::stringFormat("Error processing GetDevice request: %s", ex.what()), spdlog::level::err);
            return grpc::Status(grpc::StatusCode::INTERNAL, ex.what());
        }


        return grpc::Status::OK;
    }
    

    grpc::Status DeviceService::ListUserDevices(grpc::ServerContext * context, const acl::rpc::e2ee::v1::ListUserDevicesRequest * request, acl::rpc::e2ee::v1::ListUserDevicesResponse * response)
    {
        BCService()->LogMessage(cpp::utils::stringFormat("Service request: %s::%s", __CLASS_NAME_CSTR__, __METHOD_NAME_CSTR__));
        auto dbInstance = (acl::logos::core::db::DatabaseManager*)(BCService()->DatabaseManager());

        try {
            const auto devices = dbInstance->Devices().RetrieveByUserId(request->user_id().value());

            for (const auto& device : devices)
            {
                auto* proto_device = response->add_devices();
                proto_device->mutable_id()->set_value(device.id);
                proto_device->set_name(device.device_name);
                proto_device->set_type( static_cast<acl::rpc::e2ee::v1::DeviceType>(device.type) );
                proto_device->set_active(device.active);

                *proto_device->mutable_created_at() = ToProtoTimestamp(device.created_at);
                *proto_device->mutable_last_seen_at() = ToProtoTimestamp(device.last_seen_at);
            }
            return grpc::Status::OK;
        }
        catch (const std::exception& ex)
        {
            BCService()->LogMessage(cpp::utils::stringFormat("Error processing ListUserDevices request: %s", ex.what()), spdlog::level::err);
            return grpc::Status(grpc::StatusCode::INTERNAL, ex.what());
        }

        return grpc::Status::OK;
    }


    grpc::Status DeviceService::UpdateDevice(grpc::ServerContext * context, const acl::rpc::e2ee::v1::UpdateDeviceRequest * request, acl::rpc::e2ee::v1::UpdateDeviceResponse * response)
    {
        BCService()->LogMessage(cpp::utils::stringFormat("Service request: %s::%s", __CLASS_NAME_CSTR__, __METHOD_NAME_CSTR__));
        auto dbInstance = (acl::logos::core::db::DatabaseManager*)(BCService()->DatabaseManager());

        try {

            if(validateUpdateDeviceRequest(request))
            {
                auto device = dbInstance->Devices().Retrieve(request->device_id().value());
            
                // Current model doesn't have a device_name field.
                // Uncomment this when added.
                if (request->has_device_name())
                    device.device_name = request->device_name();

                if (request->has_active())
                    device.active = request->active();
                
                dbInstance->Devices().Update(device);
                auto* proto_device = response->mutable_device();

                proto_device->mutable_id()->set_value(device.id);
                proto_device->set_name(device.device_name);
                proto_device->set_type( static_cast<acl::rpc::e2ee::v1::DeviceType>(device.type) );
                proto_device->set_active(device.active);

                *proto_device->mutable_created_at() = ToProtoTimestamp(device.created_at);
                *proto_device->mutable_last_seen_at() = ToProtoTimestamp(device.last_seen_at);

            } else {
                // TODO
            }
            return grpc::Status::OK;
        }
        catch (const std::exception& ex)
        {
            BCService()->LogMessage(cpp::utils::stringFormat("Error processing UpdateDevice request: %s", ex.what()), spdlog::level::err);
            return grpc::Status(grpc::StatusCode::INTERNAL, ex.what());
        }
        return grpc::Status::OK;
    }


    grpc::Status DeviceService::RemoveDevice(grpc::ServerContext * context, const acl::rpc::e2ee::v1::RemoveDeviceRequest * request, google::protobuf::Empty * response)
    {
        BCService()->LogMessage(cpp::utils::stringFormat("Service request: %s::%s", __CLASS_NAME_CSTR__, __METHOD_NAME_CSTR__));
        auto dbInstance = (acl::logos::core::db::DatabaseManager*)(BCService()->DatabaseManager());

        try {
            auto device = dbInstance->Devices().Retrieve(request->device_id().value());
            dbInstance->Devices().Delete(device);
            return grpc::Status::OK;
        }
        catch (const std::exception& ex)
        {
            BCService()->LogMessage(cpp::utils::stringFormat("Error processing RemoveDevice request: %s", ex.what()), spdlog::level::err);
            return grpc::Status(grpc::StatusCode::INTERNAL, ex.what());
        }
        return grpc::Status::OK;
    }


    grpc::Status DeviceService::TouchDevice(grpc::ServerContext * context, const acl::rpc::e2ee::v1::TouchDeviceRequest * request, google::protobuf::Empty * response)
    {
        BCService()->LogMessage(cpp::utils::stringFormat("Service request: %s::%s", __CLASS_NAME_CSTR__, __METHOD_NAME_CSTR__));
        auto dbInstance = (acl::logos::core::db::DatabaseManager*)(BCService()->DatabaseManager());

        try {
            auto device = dbInstance->Devices().Retrieve(request->device_id().value());
            device.last_seen_at = std::chrono::system_clock::now();

            dbInstance->Devices().Update(device);
            return grpc::Status::OK;
        }
        catch (const std::exception& ex)
        {
            BCService()->LogMessage(cpp::utils::stringFormat("Error processing TouchDevice request: %s", ex.what()), spdlog::level::err);
            return grpc::Status(grpc::StatusCode::INTERNAL, ex.what());
        }

        return grpc::Status::OK;
    }
    


    bool DeviceService::validateRegisterDeviceRequest(const acl::rpc::e2ee::v1::RegisterDeviceRequest * request)
    {


        return true;
    }

    bool DeviceService::validateUpdateDeviceRequest(const acl::rpc::e2ee::v1::UpdateDeviceRequest * request)
    {


        return true;
    }
    
} } } }