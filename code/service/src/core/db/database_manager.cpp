#include "core/db/database_manager.h"

namespace acl { namespace logos { namespace core { namespace db {

    void DatabaseManager::LoadEntityManagers(iblockchain_node<LogosSvcSettings> * bc_node)
    {
        identities = new db::EntityManager<LogosSvcSettings, db::Identity>(bc_node, *SQLSession().get());
        wallet_keys = new db::EntityManager<LogosSvcSettings, db::WalletKey>(bc_node, *SQLSession().get());
        wallets = new db::EntityManager<LogosSvcSettings, db::Wallet>(bc_node, *SQLSession().get());
        transfers = new db::EntityManager<LogosSvcSettings, db::Transfer>(bc_node, *SQLSession().get());
    }




}}}}