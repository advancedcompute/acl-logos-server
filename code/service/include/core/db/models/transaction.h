#pragma once

#include <string>

namespace acl { namespace logos { namespace core { namespace db {

    struct Transaction
    {
        std::string tx_hash;
        std::string block_hash;

        std::string from_address;
        std::string to_address;

        std::string payload; // protobuf serialized Transaction message

        int64_t nonce;
        int64_t timestamp;

        std::string status; // pending, committed, failed
    };


}}}}