#pragma once

#include "core/db/models/_traits.h"
#include "soci/soci.h"

namespace acl { namespace logos { namespace core {

    class idatabase_manager
    {
        public:

            std::shared_ptr<soci::session>& SQLSession() { return _sql_session; }

        private:
            std::shared_ptr<soci::session> _sql_session = nullptr;
    };

} } }