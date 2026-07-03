#pragma once

#include <vector>
#include "core/interfaces/database.h"
#include "core/db/models/identity.h"
#include "core/db/models/wallet.h"

#include "entity_manager.h"
#include "core/config/cmd_arg_options.h"

namespace acl { namespace logos { namespace core { namespace db {

    class DatabaseManager: public idatabase_manager
    {
        public:

            virtual ~DatabaseManager()
            {
                if(identities) delete identities;
                if(wallet_keys) delete wallet_keys;
                if(wallets) delete wallets;
                if(transfers) delete transfers;
            }

            std::shared_ptr<soci::session>& SQLSession() { return idatabase_manager::SQLSession(); }    // Somewhat of a hack, but it does the job

            void LoadEntityManagers(iblockchain_node<LogosSvcSettings> * bc_node);



            db::EntityManager<LogosSvcSettings, db::Identity>& Identities() { return *identities; }
            db::EntityManager<LogosSvcSettings, db::WalletKey>& WalletKeys() { return *wallet_keys; }
            db::EntityManager<LogosSvcSettings, db::Wallet>& Wallets() { return *wallets; }
            db::EntityManager<LogosSvcSettings, db::Transfer>& Transfers() { return *transfers; }


        private:
            db::EntityManager<LogosSvcSettings, db::Identity> * identities = nullptr;
            db::EntityManager<LogosSvcSettings, db::WalletKey> * wallet_keys = nullptr;
            db::EntityManager<LogosSvcSettings, db::Wallet> * wallets = nullptr;
            db::EntityManager<LogosSvcSettings, db::Transfer> * transfers = nullptr;


    };


} } } }