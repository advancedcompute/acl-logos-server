import grpc

from acl.rpc.v1 import identity_pb2
from acl.rpc.v1 import identity_pb2_grpc

grpc_certpath = "/home/dominic/.config/acl/logos/node/tls/cert.pem"

def list_identities(
    server_address: str,
    ca_cert_path: str,
    client_cert_path: str = None,
    client_key_path: str = None,
    page_size: int = 10,
    page_token: str = "",
):
    # Load CA cert
    trusted_certs = None
    if ca_cert_path is not None:
        with open(ca_cert_path, "rb") as f:
            trusted_certs = f.read()

    # Optional mTLS
    if client_cert_path and client_key_path:
        with open(client_cert_path, "rb") as f:
            client_cert = f.read()

        with open(client_key_path, "rb") as f:
            client_key = f.read()

        credentials = grpc.ssl_channel_credentials(
            root_certificates=trusted_certs,
            private_key=client_key,
            certificate_chain=client_cert,
        )
    else:
        if trusted_certs is not None:
            credentials = grpc.ssl_channel_credentials(
                root_certificates=trusted_certs
            )
        else:
            credentials = grpc.ssl_channel_credentials()

    with grpc.secure_channel(server_address, credentials) as channel:
        stub = identity_pb2_grpc.IdentityServiceStub(channel)

        request = identity_pb2.ListIdentitiesRequest(
            page_size=page_size,
            page_token=page_token,
        )

        try:
            response = stub.ListIdentities(request)

            print(f"\nRetrieved {len(response.identities)} identities\n")

            for i, identity in enumerate(response.identities, start=1):
                print(f"--- Identity {i} ---")
                print(f"ID: {identity.identity_id}")
                print(f"Type: {identity.type}")
                print(f"Public Key: {identity.public_key}")
                print(f"Verified: {identity.verified}")
                print(f"Active: {identity.active}")
                print(f"Created: {identity.created_ts}")

                if identity.metadata:
                    print("Metadata:")
                    for k, v in identity.metadata.items():
                        print(f"  {k}: {v}")

                print()

            if response.next_page_token:
                print(f"Next page token: {response.next_page_token}")

        except grpc.RpcError as e:
            print(f"RPC failed: {e.code().name}")
            print(e.details())


if __name__ == "__main__":
    # Local dev
    #list_identities(
    #    server_address="localhost:50051",
    #    ca_cert_path=grpc_certpath,
    #    page_size=20,
    #)

    # Cloud-hosted
    list_identities(
        server_address="acl-logos-1027055074837.europe-west2.run.app:443",
        ca_cert_path=None
    )