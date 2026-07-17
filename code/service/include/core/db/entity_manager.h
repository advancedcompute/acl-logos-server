#pragma once
#include <soci/soci.h>
#include <string>
#include <vector>
#include <optional>

#include "macros.h"
#include "string_helpers.h"

#include "core/interfaces/blockchain_node.h"

namespace acl { namespace logos { namespace core { namespace db {

    template<typename SettingsObject, typename SQLModel>
    class EntityManager
    {
    public:

        explicit EntityManager(iblockchain_node<SettingsObject> * bc_node, soci::session& sql)
            : _bc_node(bc_node), m_sql(sql)
        {
            assert(bc_node != nullptr);
        }

        /// Creates the backing table if it does not already exist.
        bool CreateTable()
        {
            bool success = false;
            soci::transaction tx(m_sql);
            try {
                m_sql << TableTraits<SQLModel>::CreateTable();
                tx.commit();
                success = true;
            } catch(soci::soci_error& ex) {
                _bc_node->LogMessage(cpp::utils::stringFormat("DB Error (%s::%s): %s", __CLASS_NAME_CSTR__, __METHOD_NAME_CSTR__, ex.what()), spdlog::level::err);
                tx.rollback();
            }
            return success;
        }

        /// Inserts a single entity.
        bool Insert(const SQLModel& item)
        {
            bool success = false;
            soci::transaction tx(m_sql);
            try {
                TableTraits<SQLModel>::Insert(m_sql, item);
                tx.commit();
                success = true;
            } catch(soci::soci_error& ex) {
                _bc_node->LogMessage(cpp::utils::stringFormat("DB Error (%s::%s): %s", __CLASS_NAME_CSTR__, __METHOD_NAME_CSTR__, ex.what()), spdlog::level::err);
                tx.rollback();
            }
            return success;
        }

        /// Inserts a collection of entities.
        void Insert(const std::vector<SQLModel>& items)
        {
            for (const auto& item : items)
            {
                try {
                    TableTraits<SQLModel>::Insert(m_sql, item);
                } catch(soci::soci_error& ex) {
                    _bc_node->LogMessage(cpp::utils::stringFormat("DB Error (%s::%s): %s", __CLASS_NAME_CSTR__, __METHOD_NAME_CSTR__, ex.what()), spdlog::level::err);
                }
            }
        }

        /// Retrieves an entity by its primary key.
        SQLModel Retrieve(uint64_t id)
        {
            try {
                return TableTraits<SQLModel>::Retrieve(m_sql, id);
            } catch(soci::soci_error& ex) {
                _bc_node->LogMessage(cpp::utils::stringFormat("DB Error (%s::%s): %s", __CLASS_NAME_CSTR__, __METHOD_NAME_CSTR__, ex.what()), spdlog::level::err);
                throw std::runtime_error(ex.what());
            }
        }

        /// Retrieves every entity in the table.
        std::vector<SQLModel> RetrieveAll()
        {
            std::vector<SQLModel> retSet;
            try {
                return TableTraits<SQLModel>::RetrieveAll(m_sql);
            } catch(soci::soci_error& ex) {
                _bc_node->LogMessage(cpp::utils::stringFormat("DB Error (%s::%s): %s", __CLASS_NAME_CSTR__, __METHOD_NAME_CSTR__, ex.what()), spdlog::level::err);
            }
            return retSet;
        }

        /// Updates an entity.
        void Update(const SQLModel& item)
        {
            soci::transaction tx(m_sql);
            try {
                TableTraits<SQLModel>::Update(m_sql, item);
                tx.commit();
            } catch(soci::soci_error& ex) {
                _bc_node->LogMessage(cpp::utils::stringFormat("DB Error (%s::%s): %s", __CLASS_NAME_CSTR__, __METHOD_NAME_CSTR__, ex.what()), spdlog::level::err);
                tx.rollback();
            }
        }

        /// Updates a collection of entities.
        void Update(const std::vector<SQLModel>& items)
        {
            for (const auto& item : items)
            {
                try {
                    TableTraits<SQLModel>::Update(m_sql, item);
                } catch(soci::soci_error& ex) {
                    _bc_node->LogMessage(cpp::utils::stringFormat("DB Error (%s::%s): %s", __CLASS_NAME_CSTR__, __METHOD_NAME_CSTR__, ex.what()), spdlog::level::err);
                }
            }
        }

        /// Deletes an entity.
        void Delete(const SQLModel& item)
        {
            soci::transaction tx(m_sql);
            try {
                TableTraits<SQLModel>::Delete(m_sql, item);
                tx.commit();
            } catch(soci::soci_error& ex) {
                _bc_node->LogMessage(cpp::utils::stringFormat("DB Error (%s::%s): %s", __CLASS_NAME_CSTR__, __METHOD_NAME_CSTR__, ex.what()), spdlog::level::err);
                tx.rollback();
            }
        }

        /// Deletes a collection of entities.
        void Delete(const std::vector<SQLModel>& items)
        {
            for (const auto& item : items)
            {
                try {
                    TableTraits<SQLModel>::Delete(m_sql, item);
                } catch(soci::soci_error& ex) {
                    _bc_node->LogMessage(cpp::utils::stringFormat("DB Error (%s::%s): %s", __CLASS_NAME_CSTR__, __METHOD_NAME_CSTR__, ex.what()), spdlog::level::err);
                }
            }
        }


    private:

        soci::session& m_sql;
        iblockchain_node<SettingsObject> * _bc_node = nullptr;
    };
} } } }