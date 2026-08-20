import grpc
from acl.rpc.e2ee.v1 import common_pb2
from acl.rpc.e2ee.v1 import identity_pb2
from acl.rpc.e2ee.v1 import identity_pb2_grpc

with open("/home/dominic/.config/acl/logos/node/tls/grpc.cert.pem", "rb") as f:
    root_cert = f.read()

creds = grpc.ssl_channel_credentials(root_certificates=root_cert)
channel = grpc.secure_channel("localhost:50051", creds)

# Cloud-hosted instance
# channel = grpc.secure_channel(
#     "logos.advancedcomputation.org.uk",
#     grpc.ssl_channel_credentials()
# )


with open("/home/dominic/.config/acl/logos/node/tls/grpc.cert.pem", "rb") as f:
    public_key = f.read()


stub = identity_pb2_grpc.IdentityServiceStub(channel)
request_content = identity_pb2.GetIdentityRequest(
    identity_id=common_pb2.IdentityId(
        value=1
    )
)


try:
    response = stub.GetIdentity(request_content)
    print(response)
except grpc.RpcError as e:
    print("Code:", e.code())
    print("Details:", e.details())
    print("Debug:", e.debug_error_string())