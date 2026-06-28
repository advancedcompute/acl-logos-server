#pragma once

#include <string>

namespace acl { namespace logos { namespace core { namespace db {

    struct Network
    {
        std::string network_id;
        std::string name;

        std::string genesis_hash;
        std::string config;

        bool active;
    };


}}}}