#pragma once
#include <string>
#include "user.h"
#include "device.h"


namespace acl { namespace logos { namespace core { namespace db {

    struct device_crypto_suite
    {
        user user;
        device device;
        std::string suite_id;
    };



} } } }