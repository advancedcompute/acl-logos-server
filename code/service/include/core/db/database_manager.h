#pragma once

#include <vector>
#include "core/interfaces/database.h"
#include "core/db/models/identity.h"
#include "core/db/models/wallet.h"

namespace acl { namespace logos { namespace core { namespace db {

    template <typename T>
    class EntityManager
    {
        public:

            


        private:
            std::vector<T> _cachedItems;
            std::shared_ptr<soci::session> _sql_session;
    };

    class DatabaseManager: public idatabase_manager
    {
        public:

            ischema_manager<Identity>& IdentityTable() { return _identityTable; }
            ischema_manager<Transfer>& TransferTable() { return _transferTable; }
            ischema_manager<TransferSignature> TransferSignatureTable() { return _transferSignatureTable; }
            ischema_manager<Wallet>& WalletTable() { return _walletTable; }
            ischema_manager<WalletKey>& WalletKeyTable() { return _walletKeyTable; }

            std::shared_ptr<soci::session>& SQLSession() { return idatabase_manager::SQLSession(); }    // Somewhat of a hack, but it does the job


        private:
            ischema_manager<Identity> _identityTable;
            ischema_manager<Wallet> _walletTable;
            ischema_manager<WalletKey> _walletKeyTable;
            ischema_manager<Transfer> _transferTable;
            ischema_manager<TransferSignature> _transferSignatureTable;

    };


} } } }