#pragma once

#include <string>

namespace acl { namespace logos { namespace core { namespace db {

    struct Block
    {
        std::string hash;
        std::string prev_hash;
        int64_t height;
        int64_t timestamp;

        std::string merkle_root;
        std::string proposer;

        std::string raw_data; // serialized block (important for flexibility)
    };


}}}}