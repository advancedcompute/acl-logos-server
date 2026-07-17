#pragma once

#include <soci/soci.h>
#include "_traits.h"
#include <cstdint>
#include <string>
#include <vector>
#include <json/json.h>
//#include "file.h"
#include <chrono>
#include "one_time_pre_key.h"


namespace acl { namespace logos { namespace core { namespace db {

    struct identity
    {
        uint64_t device_id;
        int algorithm;
        std::vector<uint8_t> public_key;
        //std::vector<uint64_t> one_time_pre_keys;          // TODO: Move to another table. 1-to-N relationship
        std::chrono::system_clock::time_point created_at;
    };


    template<>
    struct TableTraits<identity>
    {
        static std::string CreateTable()
        {
            return R"(
                CREATE TABLE IF NOT EXISTS identities
                (
                    device_id   INTEGER PRIMARY KEY,
                    algorithm   INTEGER NOT NULL,
                    public_key  BLOB NOT NULL,
                    created_at  INTEGER NOT NULL,
                    FOREIGN KEY(device_id) REFERENCES devices(id)
                );
            )";
        }


        static void Insert(soci::session& sql, const identity& value)
        {
            auto created_at = std::chrono::system_clock::to_time_t(value.created_at);
            sql <<
                R"(
                    INSERT INTO identities
                    (
                        device_id,
                        algorithm,
                        public_key,
                        created_at
                    )
                    VALUES
                    (
                        :device_id,
                        :algorithm,
                        :public_key,
                        :created_at
                    )
                )",
                soci::use(value.device_id),
                soci::use(value.algorithm),
                soci::use(value.public_key),
                soci::use(created_at);
        }


        static identity Retrieve(soci::session& sql, uint64_t device_id)
        {
            identity value;
            std::time_t created_at;

            sql <<
                R"(
                    SELECT
                        device_id,
                        algorithm,
                        public_key,
                        created_at
                    FROM identities
                    WHERE device_id = :device_id
                )",
                soci::into(value.device_id),
                soci::into(value.algorithm),
                soci::into(value.public_key),
                soci::into(created_at),
                soci::use(device_id);

            value.created_at = std::chrono::system_clock::from_time_t(created_at);

            /*
            // Future:
            // Load one_time_pre_keys here
            value.one_time_pre_keys =
                OneTimePreKeyRepository::RetrieveForIdentity(
                    sql,
                    value.device_id);
            */
            return value;
        }


        static void Update(soci::session& sql, const identity& value)
        {
            auto created_at = std::chrono::system_clock::to_time_t(value.created_at);

            sql <<
                R"(
                    UPDATE identities
                    SET
                        algorithm = :algorithm,
                        public_key = :public_key,
                        created_at = :created_at
                    WHERE device_id = :device_id
                )",
                soci::use(value.algorithm),
                soci::use(value.public_key),
                soci::use(created_at),
                soci::use(value.device_id);
        }


        static void Delete(soci::session& sql, const identity& value)
        {
            sql << "DELETE FROM identities WHERE device_id = :device_id", soci::use(value.device_id);
        }


        static std::vector<identity> RetrieveAll(soci::session& sql)
        {
            std::vector<identity> identities;
            soci::rowset<soci::row> rows =
                (sql.prepare <<
                    R"(
                        SELECT
                            device_id,
                            algorithm,
                            public_key,
                            created_at
                        FROM identities
                    )");
            
            for (const auto& row : rows)
            {
                identity value;
                std::time_t created_at = row.get<std::time_t>(3);
                value.device_id = row.get<uint64_t>(0);
                value.algorithm = row.get<int>(1);
                value.public_key = row.get<std::vector<uint8_t>>(2);
                value.created_at = std::chrono::system_clock::from_time_t(created_at);

                /*
                // Future:
                // Load one_time_pre_keys here
                */
                identities.push_back(std::move(value));
            }
            return identities;
        }

    };


}}}}