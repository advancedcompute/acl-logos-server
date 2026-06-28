#pragma once

#include <string>

namespace acl { namespace logos { namespace core { namespace db {

    struct Identity
    {
        std::string user_id;
        std::string public_key;
        std::string did;

        int64_t created_at;
    };


}}}}