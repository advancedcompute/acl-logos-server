#pragma once

#include <string>

namespace acl { namespace logos { namespace core { namespace db {

    struct Peer
    {
        std::string peer_id;
        std::string address;

        int64_t last_seen;
        bool active;

        std::string metadata;
    };


}}}}