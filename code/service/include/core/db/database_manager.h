#pragma once

#include <vector>
#include "core/interfaces/database.h"
#include "core/db/models/device.h"
#include "core/db/models/identity.h"
#include "core/db/models/device_identity_key.h"
#include "core/db/models/signed_pre_key.h"
#include "core/db/models/one_time_pre_key.h"
#include "core/db/models/user.h"

#include "entity_manager.h"
#include "core/config/cmd_arg_options.h"

namespace acl { namespace logos { namespace core { namespace db {

    class DatabaseManager: public idatabase_manager
    {
        public:

            virtual ~DatabaseManager()
            {
                if(users) delete users;
                if(identities) delete identities;
                if(devices) delete devices;

                if(device_identity_keys) delete device_identity_keys;
                if(one_time_pre_keys) delete one_time_pre_keys;
                if(signed_pre_keys) delete signed_pre_keys;
            }

            std::shared_ptr<soci::session>& SQLSession() { return idatabase_manager::SQLSession(); }    // Somewhat of a hack, but it does the job

            void LoadEntityManagers(iblockchain_node<LogosSvcSettings> * bc_node);

            db::EntityManager<LogosSvcSettings, db::user>& Users() { return *users; }
            db::EntityManager<LogosSvcSettings, db::identity>& Identities() { return *identities; }
            db::EntityManager<LogosSvcSettings, db::device>& Devices() { return *devices; }
            
            db::EntityManager<LogosSvcSettings, db::device_identity_key>& DeviceIdentityKeys() { return *device_identity_keys; }
            db::EntityManager<LogosSvcSettings, db::one_time_pre_key>& OneTimePreKeys() { return *one_time_pre_keys; }
            db::EntityManager<LogosSvcSettings, db::signed_pre_key>& SignedPreKeys() { return *signed_pre_keys; }

        private:
            db::EntityManager<LogosSvcSettings, db::user> * users = nullptr;
            db::EntityManager<LogosSvcSettings, db::device> * devices = nullptr;
            db::EntityManager<LogosSvcSettings, db::identity> * identities = nullptr;

            db::EntityManager<LogosSvcSettings, db::device_identity_key> * device_identity_keys = nullptr;
            db::EntityManager<LogosSvcSettings, db::one_time_pre_key> * one_time_pre_keys = nullptr;
            db::EntityManager<LogosSvcSettings, db::signed_pre_key> * signed_pre_keys = nullptr;
    };


} } } }