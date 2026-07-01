import grpc
from google.protobuf.empty_pb2 import Empty

from acl.rpc import status_pb2
from acl.rpc import status_pb2_grpc

with open("/home/dominic/.config/acl/logos/node/tls/cert.pem", "rb") as f:
    root_cert = f.read()

creds = grpc.ssl_channel_credentials(root_certificates=root_cert)
channel = grpc.secure_channel("localhost:50051", creds)
stub = status_pb2_grpc.StatusStub(channel)

try:
    response = stub.Up(Empty())
    print(response)
except grpc.RpcError as e:
    print("Code:", e.code())
    print("Details:", e.details())
    print("Debug:", e.debug_error_string())