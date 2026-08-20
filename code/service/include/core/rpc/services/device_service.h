#pragma once

#include <grpc/grpc.h>
#include <string>

#include "core/config/cmd_arg_options.h"
#include "core/interfaces/blockchain_node.h"
#include "core/rpc/generated/device.grpc.pb.h"

namespace acl { namespace logos { namespace core { namespace rpc {

    class DeviceService final:
        public acl::logos::core::iblockchain_node_service<LogosSvcSettings>,
        public acl::rpc::e2ee::v1::DeviceService::Service
    {
    public:
        explicit DeviceService(iblockchain_node<LogosSvcSettings> * nodePtr):
            acl::rpc::e2ee::v1::DeviceService::Service(),
            iblockchain_node_service(nodePtr, this)
        {

        }
        ~DeviceService() override {}
        
        grpc::Status RegisterDevice(grpc::ServerContext * context, const acl::rpc::e2ee::v1::RegisterDeviceRequest * request, acl::rpc::e2ee::v1::RegisterDeviceResponse * response);
        
        grpc::Status GetDevice(grpc::ServerContext * context, const acl::rpc::e2ee::v1::GetDeviceRequest * request, acl::rpc::e2ee::v1::GetDeviceResponse * response);

        grpc::Status ListUserDevices(grpc::ServerContext * context, const acl::rpc::e2ee::v1::ListUserDevicesRequest * request, acl::rpc::e2ee::v1::ListUserDevicesResponse * response);

        grpc::Status UpdateDevice(grpc::ServerContext * context, const acl::rpc::e2ee::v1::UpdateDeviceRequest * request, acl::rpc::e2ee::v1::UpdateDeviceResponse * response);

        grpc::Status RemoveDevice(grpc::ServerContext * context, const acl::rpc::e2ee::v1::RemoveDeviceRequest * request, google::protobuf::Empty * response);

        grpc::Status TouchDevice(grpc::ServerContext * context, const acl::rpc::e2ee::v1::TouchDeviceRequest * request, google::protobuf::Empty * response);

    private:

        bool validateRegisterDeviceRequest(const acl::rpc::e2ee::v1::RegisterDeviceRequest * request);

        bool validateUpdateDeviceRequest(const acl::rpc::e2ee::v1::UpdateDeviceRequest * request);
        
    };

} } } }