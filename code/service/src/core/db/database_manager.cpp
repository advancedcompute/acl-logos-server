#include "core/db/database_manager.h"

namespace acl { namespace logos { namespace core { namespace db {

    void DatabaseManager::LoadEntityManagers(iblockchain_node<LogosSvcSettings> * bc_node)
    {
        users = new db::EntityManager<LogosSvcSettings, db::user>(bc_node, *SQLSession().get());
        devices = new db::EntityManager<LogosSvcSettings, db::device>(bc_node, *SQLSession().get());
        identities = new db::EntityManager<LogosSvcSettings, db::identity>(bc_node, *SQLSession().get());
        //users = new db::EntityManager<LogosSvcSettings, db::user>(bc_node, *SQLSession().get());
    }


}}}}