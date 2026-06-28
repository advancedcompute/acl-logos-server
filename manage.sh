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

    declare -a service_filepath_list=$(ls $1/*.proto)
    declare -a service_filename_list=$(find $1/*.proto -printf "%f\n")

    for service_filepath in $service_filepath_list; do
        service_full_filename="$(basename $service_filepath)"
        service_filename="${service_full_filename%.*}"
        python3 -m grpc_tools.protoc -I $1 --python_out=$2 --grpc_python_out=$2 "$service_filepath"

        # Work around fix to prefix the imports
        #for svc_filename in $service_filename_list; do
        #    svc_name="${svc_filename%.*}"
        #    sed -i "s/import ${svc_name}/import dte.${svc_name}/g" "$2/${service_filename}_pb2_grpc.py"
        #    sed -i "s/import ${svc_name}/import dte.${svc_name}/g" "$2/${service_filename}_pb2.py"
        #done
    done
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
        ["generate-cpp"]="Generate C++ gRPC interface code for services"
        ["generate-py"]="Generate python gRPC interface code for services"
        ["clean-build"]="Clean build directories"
        ["clean-code"]="Clean generated code (python & c++)"
        ["clean"]="        Clean certs, generated code & build directories"
        ["help"]="        Display command usage")

    echo "manage.sh - Script to manage source code repository actions"
    echo "Arguments:"

    for key in "${!commands[@]}"; do
        printf "\t%s\t%s\n" "${key}" "${commands[$key]}"
    done
}



if [ "$1" = "generate-py" ]; then
    if [ ! -d "$script_dir/.grpc_generated/py" ]; then
        mkdir "$script_dir/.grpc_generated/py"
    fi;
    generatePython $script_dir/dep/acl-blockchain-proto/protobuf $script_dir/.grpc_generated/py
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
elif [ "$1" == "help" ]; then
    usage
else
    if [ -z "$1" ]; then
        usage
    else
        echo "Unrecognised argument: $1"
    fi;
fi;