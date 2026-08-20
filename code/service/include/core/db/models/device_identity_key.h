#pragma once

#include <soci/soci.h>
#include "_traits.h"
#include <string>
#include <vector>
//#include "device.h"


namespace acl { namespace logos { namespace core { namespace db {

    struct device_identity_key
    {
        uint64_t id;
        //std::vector<device> devices;
    };

} } } }