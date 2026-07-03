
import grpc, json
from acl.rpc.v1 import identity_pb2
from acl.rpc.v1 import identity_pb2_grpc

ecc_keypath = '/home/dominic/Documents/work/acl-logos-server/etc/demo_ecc_public_key.pem'
grpc_certpath = "/home/dominic/.config/acl/logos/node/tls/cert.pem"

id_request_type = identity_pb2.COMPANY
id_request_metadata = {
    "first_name": "Advanced Computation Laboratory",
    "last_name": "Org",
    "email": "administrator@advancedcomputation.org.uk",
}


def register_identity(server_address: str, ca_cert_path: str):
    # Create TLS credentials
    with open(grpc_certpath, "rb") as f:
        root_cert = f.read()
    credentials = grpc.ssl_channel_credentials(root_certificates=root_cert)

    ecc_pem = None
    with open(ecc_keypath) as fp:
        ecc_pem = fp.read()
    #print(ecc_pem)

    # Create secure channel
    with grpc.secure_channel(server_address, credentials) as channel:
        stub = identity_pb2_grpc.IdentityServiceStub(channel)

        request = identity_pb2.RegisterIdentityRequest(
            type=id_request_type,
            public_key=ecc_pem,
            metadata=id_request_metadata)

        try:
            response = stub.RegisterIdentity(request)

            print("Identity registered successfully")
            print(response.identity)

            if response.errors:
                print("Errors:")
                for err in response.errors:
                    print(err)

        except grpc.RpcError as e:
            print(f"RPC failed: {e.code().name}")
            print(e.details())


if __name__ == "__main__":
    register_identity(
        server_address="localhost:50051",
        ca_cert_path=grpc_certpath,
    )