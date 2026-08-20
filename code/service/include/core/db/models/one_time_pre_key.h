#pragma once
#include "user.h"
#include "device.h"
#include <string>


namespace acl { namespace logos { namespace core { namespace db {

    struct one_time_pre_key
    {
        uint64_t user_id;
        uint64_t device_id;
        std::string key_id;
    };
    

} } } }