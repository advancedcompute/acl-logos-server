#pragma once

#include <string>

namespace acl { namespace logos { namespace core { namespace db {

    struct ConsensusState
    {
        std::string round_id;
        int64_t height;

        std::string leader;
        std::string phase; // propose / vote / commit

        std::string data;
    };


}}}}