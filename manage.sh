#!/bin/bash

# Global variables
script_dir=$(dirname "$0")
declare -a environments=("dev")    #, "test", "stage", "prod")                        # Different deployment settings etc


function cleanDirectory() {
    if [ -d "$1" ]; then
        rm -rf "$1"
    fi;

    if [ -z "$2" ]; then
        mkdir "$1"
    fi;
}

function cleanGeneratedCode() {
    cleanDirectory $script_dir/.grpc_generated
    cleanDirectory $script_dir/code/service/include/core/rpc/generated
    cleanDirectory $script_dir/code/service/src/core/rpc/generated
}

function cleanBuildDirs() {
    cleanDirectory $script_dir/build/ 1
    cleanDirectory $script_dir/cmake-build-debug/ 1
    cleanDirectory $script_dir/cmake-build-release/ 1
}


function generatePython() {
    # Make the output directory
    if [ ! -d "$2" ]; then
        mkdir "$2"
    fi;

    python3 -m grpc_tools.protoc \
        -I "$1" \
        --python_out="$2" \
        --grpc_python_out="$2" \
        "$1"/*.proto
}

function generateCpp() {
    if [ ! -d "$2" ]; then
         mkdir "$2"
    fi;

    for file in $1/*.proto; do
        #echo protoc -I $1 --grpc_out=$2 --cpp_out=$2 --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` "$file"
        protoc -I $1 --grpc_out=$2 --cpp_out=$2 --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` "$file"
    done
}

function usage() {

    declare -A commands=(
        ["generate-demo-certs"]="Generate self-signed demo certs"
        ["generate-cpp"]="Generate C++ gRPC interface code for services"
        ["generate-py"]="Generate python gRPC interface code for services"
        ["clean-build"]="Clean build directories"
        ["clean-demo-certs"]="Clean self-signed demo certs"
        ["clean-code"]="Clean generated code (python & c++)"
        ["clean"]="        Clean certs, generated code & build directories"
        ["help"]="        Display command usage")

    echo "manage.sh - Script to manage source code repository actions"
    echo "Arguments:"

    for key in "${!commands[@]}"; do
        printf "\t%s\t%s\n" "${key}" "${commands[$key]}"
    done
}


if [ "$1" = "generate-demo-certs" ]; then
    openssl ecparam -name prime256v1 -genkey -noout -out ./etc/demo_ecc_private_key.pem
    openssl req \
        -new -key ./etc/demo_ecc_private_key.pem \
        -out ./etc/demo_ecc_csr.pem \
        -subj "/C=GB/ST=Cambridgeshire/L=Cambridge/O=ACL/CN=localhost/emailAddress=administrator@advancedcomputation.org.uk"
    openssl req -x509 -key ./etc/demo_ecc_private_key.pem \
        -days 365 -out ./etc/demo_ecc_certificate.pem   \
        -subj "/C=GB/ST=Cambridgeshire/L=Cambridge/O=ACL/CN=localhost/emailAddress=administrator@advancedcomputation.org.uk"
    openssl pkey -in ./etc/demo_ecc_private_key.pem -pubout -outform pem > ./etc/demo_ecc_public_key.pem

    if [ "$2" = 1 ]; then
        mkdir -p ~/.config/acl/logos/node/tls/
        cp ./etc/demo_ecc_certificate.pem ~/.config/acl/logos/node/tls/cert.pem
        cp ./etc/demo_ecc_private_key.pem ~/.config/acl/logos/node/tls/key.pem
        cp ./etc/demo_ecc_public_key.pem ~/.config/acl/logos/node/tls/ca.pem
    fi;

elif [ "$1" = "generate-py" ]; then
    if [ ! -d "$script_dir/.grpc_generated/py" ]; then
        mkdir -p "$script_dir/.grpc_generated/py"
    fi;
    generatePython $script_dir/dep/acl-blockchain-proto/protobuf $script_dir/.grpc_generated/py

    if [ ! -d "$script_dir/.grpc_generated/acl/rpc" ]; then
        mkdir -p "$script_dir/.grpc_generated/acl/rpc"
    fi;
    cp -r $script_dir/.grpc_generated/py/* $script_dir/.grpc_generated/acl/rpc

    # Work around fix to prefix the imports
    find "$script_dir/.grpc_generated/acl/rpc" -name '*.py' -exec \
        sed -Ei 's/^import ([a-zA-Z0-9_]+_pb2)( as )?/from . import \1\2/' {} \;
    find "$script_dir/.grpc_generated/acl/rpc" -name '*.py' -exec \
        sed -Ei 's/^import ([a-zA-Z0-9_]+_pb2_grpc)( as )?/from . import \1\2/' {} \;
    
    # Add __init__.py files
    touch $script_dir/.grpc_generated/acl/__init__.py
    touch $script_dir/.grpc_generated/acl/rpc/__init__.py
elif [ "$1" = "generate-cpp" ]; then
    if [ ! -d "$script_dir/.grpc_generated/cpp" ]; then
        mkdir "$script_dir/.grpc_generated/cpp"
    fi;
    generateCpp $script_dir/dep/acl-blockchain-proto/protobuf $script_dir/.grpc_generated/cpp
    cp $script_dir/.grpc_generated/cpp/*.h $script_dir/code/service/include/core/rpc/generated
    cp $script_dir/.grpc_generated/cpp/*.cc $script_dir/code/service/src/core/rpc/generated
elif [ "$1" == "clean-build" ]; then
    cleanBuildDirs
elif [ "$1" == "clean-code" ]; then
    cleanGeneratedCode
elif [ "$1" == "clean-demo-certs" ]; then
    rm ./etc/demo_ecc_*.pem
elif [ "$1" == "help" ]; then
    usage
else
    if [ -z "$1" ]; then
        usage
    else
        echo "Unrecognised argument: $1"
    fi;
fi;