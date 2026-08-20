import grpc
from acl.rpc.e2ee.v1 import common_pb2
from acl.rpc.e2ee.v1 import device_pb2
from acl.rpc.e2ee.v1 import device_pb2_grpc

with open("/home/dominic/.config/acl/logos/node/tls/grpc.cert.pem", "rb") as f:
    root_cert = f.read()

creds = grpc.ssl_channel_credentials(root_certificates=root_cert)
channel = grpc.secure_channel("localhost:50051", creds)            # Local dev

# Cloud-hosted instance
#channel = grpc.secure_channel(
#    #1783643491"acl-logos-1027055074837.europe-west2.run.app:443",
#    "logos.advancedcomputation.org.uk",
#    grpc.ssl_channel_credentials()
#)

stub = device_pb2_grpc.DeviceServiceStub(channel)
request_content = device_pb2.RegisterDeviceRequest(
    user_id = common_pb2.UserId(
        value = 1
    ),
    client_device_id = "acl-client-device-id",
    device_name = "acl-device-name",
    type = device_pb2.DeviceType.DEVICE_TYPE_MOBILE
)

try:
    response = stub.RegisterDevice(request_content)
    print(response)
except grpc.RpcError as e:
    print("Code:", e.code())
    print("Details:", e.details())
    print("Debug:", e.debug_error_string())
