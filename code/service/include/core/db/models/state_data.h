#pragma once

#include <string>

namespace acl { namespace logos { namespace core { namespace db {

    struct StateData
    {
        std::string key;
        std::string value;

        std::string version;
        std::string block_hash;

        int64_t updated_at;
    };


}}}}