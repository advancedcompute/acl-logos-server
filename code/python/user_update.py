import grpc
from acl.rpc.e2ee.v1 import common_pb2
from acl.rpc.e2ee.v1 import user_pb2
from acl.rpc.e2ee.v1 import user_pb2_grpc

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

stub = user_pb2_grpc.UserServiceStub(channel)
request_content = user_pb2.UpdateUserRequest(
    id = common_pb2.UserId(
        value = 1
    ),
    first_name = "updated",
    last_name = "user",
    display_name = "Updated User",
    phone_number = "0987654321",
    email_address = "updated_email@internet.com"
)

try:
    response = stub.UpdateUser(request_content)
    print(response)
except grpc.RpcError as e:
    print("Code:", e.code())
    print("Details:", e.details())
    print("Debug:", e.debug_error_string())
