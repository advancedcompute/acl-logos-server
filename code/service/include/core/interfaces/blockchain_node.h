#pragma once

#include <grpc++/grpc++.h>

namespace acl { namespace logos { namespace core {

    // Parent. Inner-sanctum. 
    class iblockchain_node
    {
        public:


        private:


    };

    class iblockchain_node_service
    {
        public:
            iblockchain_node_service(grpc::Service * service = nullptr): _service(service) {}
            grpc::Service * RPCService() { return _service; }

        private:
            grpc::Service * _service = nullptr;
    };

    // Runtime scope
    class iblockchain_node_runtime
    {
        public:


        private:


    };


}
}
}