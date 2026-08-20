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
        uint64_t id;
        uint64_t device_id;
        int algorithm;
        std::string public_key;
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
                    id          INTEGER PRIMARY KEY AUTO_INCREMENT,
                    device_id   INTEGER NOT NULL,
                    algorithm   INTEGER NOT NULL,
                    public_key  BLOB NOT NULL,
                    created_at  BIGINT NOT NULL,
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


        static identity Retrieve(soci::session& sql, uint64_t id)
        {
            identity value;
            std::time_t created_at;

            soci::statement st =
            (
                sql.prepare <<
                R"(
                    SELECT
                        id,
                        device_id,
                        algorithm,
                        public_key,
                        created_at
                    FROM identities
                    WHERE id = :id
                )",
                soci::into(value.id),
                soci::into(value.device_id),
                soci::into(value.algorithm),
                soci::into(value.public_key),
                soci::into(created_at),
                soci::use(id)
            );

            st.execute(true);

            if (!st.got_data()) throw std::runtime_error("Identity not found.");

            value.created_at = std::chrono::system_clock::from_time_t(created_at);
            return value;
        }


        static void Update(soci::session& sql, const identity& value)
        {
            auto created_at =
                std::chrono::system_clock::to_time_t(value.created_at);

            sql <<
                R"(
                    UPDATE identities
                    SET
                        device_id = :device_id,
                        algorithm = :algorithm,
                        public_key = :public_key,
                        created_at = :created_at
                    WHERE id = :id
                )",
                soci::use(value.device_id),
                soci::use(value.algorithm),
                soci::use(value.public_key),
                soci::use(created_at),
                soci::use(value.id);
        }


        static void Delete(soci::session& sql, const identity& value)
        {
            sql <<
                "DELETE FROM identities WHERE id = :id",
                soci::use(value.id);
        }


        static std::vector<identity> RetrieveAll(soci::session& sql)
        {
            std::vector<identity> identities;

            soci::rowset<soci::row> rows =
                (sql.prepare <<
                    R"(
                        SELECT
                            id,
                            device_id,
                            algorithm,
                            public_key,
                            created_at
                        FROM identities
                    )");

            for (const auto& row : rows)
            {
                identity value;

                std::time_t created_at =
                    row.get<std::time_t>(4);

                value.id = row.get<uint64_t>(0);
                value.device_id = row.get<uint64_t>(1);
                value.algorithm = row.get<int>(2);
                value.public_key = row.get<std::string>(3);
                value.created_at =
                    std::chrono::system_clock::from_time_t(created_at);

                identities.push_back(std::move(value));
            }

            return identities;
        }


        static std::vector<identity> RetrieveByDeviceId(
            soci::session& sql,
            uint64_t device_id)
        {
            std::vector<identity> identities;

            soci::rowset<soci::row> rows =
                (sql.prepare <<
                    R"(
                        SELECT
                            id,
                            device_id,
                            algorithm,
                            public_key,
                            created_at
                        FROM identities
                        WHERE device_id = :device_id
                    )",
                    soci::use(device_id));

            for (const auto& row : rows)
            {
                identity value;

                std::time_t created_at =
                    row.get<std::time_t>(4);

                value.id = row.get<uint64_t>(0);
                value.device_id = row.get<uint64_t>(1);
                value.algorithm = row.get<int>(2);
                value.public_key = row.get<std::string>(3);
                value.created_at =
                    std::chrono::system_clock::from_time_t(created_at);

                identities.push_back(std::move(value));
            }

            return identities;
        }
    };


}}}}