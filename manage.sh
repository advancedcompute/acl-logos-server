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
    mkdir -p "$2"

    python3 -m grpc_tools.protoc \
        -I"$1" \
        --python_out="$2" \
        --grpc_python_out="$2" \
        $(find "$1" -name "*.proto")
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
    mkdir -p $script_dir/etc/tls

    openssl genrsa -out $script_dir/etc/tls/server-ca.key 4096
    openssl req \
        -x509 \
        -new \
        -nodes \
        -key $script_dir/etc/tls/server-ca.key \
        -sha256 \
        -days 3650 \
        -out $script_dir/etc/tls/server-ca.crt \
        -subj "/CN=ACL Testing CA"
    openssl genrsa -out $script_dir/etc/tls/server.key 4096
    openssl req \
        -new \
        -key $script_dir/etc/tls/server.key \
        -out $script_dir/etc/tls/server.csr \
        -subj "/CN=localhost"
    openssl x509 \
        -req \
        -in $script_dir/etc/tls/server.csr \
        -CA $script_dir/etc/tls/server-ca.crt \
        -CAkey $script_dir/etc/tls/server-ca.key \
        -CAcreateserial \
        -out $script_dir/etc/tls/server.crt \
        -days 365 \
        -sha256 \
        -extfile $script_dir/etc/local.server.ext


    if [ "$2" = 1 ]; then
        mkdir -p ~/.config/acl/logos/node/tls/
        cp $script_dir/etc/tls/server.crt ~/.config/acl/logos/node/tls/grpc.cert.pem
        cp $script_dir/etc/tls/server.key ~/.config/acl/logos/node/tls/grpc.key.pem
        cp $script_dir/etc/tls/server-ca.crt ~/.config/acl/logos/node/tls/grpc.ca.pem
    fi;

elif [ "$1" = "generate-py" ]; then
    generatePython $script_dir/dep/acl-blockchain-proto/protobuf \
        $script_dir/.grpc_generated/py
    
    if [ ! -d "$script_dir/.grpc_generated/acl/rpc/v1" ]; then
        mkdir -p "$script_dir/.grpc_generated/acl/rpc/v1"
    fi;
    cp -r $script_dir/.grpc_generated/py/* $script_dir/.grpc_generated/acl/rpc/v1

    # Work around fix to prefix the imports
    find "$script_dir/.grpc_generated/acl/rpc/v1" -name '*.py' -exec \
        sed -Ei 's/^import ([a-zA-Z0-9_]+_pb2)( as )?/from . import \1\2/' {} \;
    find "$script_dir/.grpc_generated/acl/rpc/v1" -name '*.py' -exec \
        sed -Ei 's/^import ([a-zA-Z0-9_]+_pb2_grpc)( as )?/from . import \1\2/' {} \;
    
    # Add __init__.py files
    touch $script_dir/.grpc_generated/acl/__init__.py
    touch $script_dir/.grpc_generated/acl/rpc/__init__.py
    touch $script_dir/.grpc_generated/acl/rpc/v1/__init__.py
elif [ "$1" = "generate-cpp" ]; then
    if [ ! -d "$script_dir/.grpc_generated/cpp" ]; then
        mkdir "$script_dir/.grpc_generated/cpp"
    fi;
    generateCpp $script_dir/dep/acl-blockchain-proto/protobuf $script_dir/.grpc_generated/cpp
    cp $script_dir/.grpc_generated/cpp/*.h $script_dir/code/service/include/core/rpc/generated
    cp $script_dir/.grpc_generated/cpp/*.cc $script_dir/code/service/src/core/rpc/generated
elif [ "$1" == "clean" ]; then
    cleanGeneratedCode
    cleanBuildDirs
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