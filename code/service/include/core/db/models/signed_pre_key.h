#pragma once
#include "user.h"
#include "device.h"
#include <chrono>
#include <vector>


namespace acl { namespace logos { namespace core { namespace db {

    struct signed_pre_key
    {
        uint64_t device_id;
        uint64_t key_id;
        std::vector<uint8_t> public_key;
        std::vector<uint8_t> signature;
        std::chrono::system_clock::time_point created_at;
    };

} } } }