#pragma once
#include "_traits.h"
#include <chrono>
#include <string>
#include <vector>
//#include "identity.h"


namespace acl { namespace logos { namespace core { namespace db {

    struct device
    {
        uint64_t id;
        uint64_t user_id;
        std::string device_id;
        std::string device_name;
        int type;
        bool active;

        std::chrono::system_clock::time_point created_at;
        std::chrono::system_clock::time_point last_seen_at;
    };

    template<>
    struct TableTraits<device>
    {
        static std::string CreateTable()
        {
            return R"(
                CREATE TABLE IF NOT EXISTS devices
                (
                    id              INTEGER AUTO_INCREMENT PRIMARY KEY,
                    user_id         INTEGER NOT NULL,
                    device_id       TEXT NOT NULL,
                    device_name     TEXT NOT NULL,
                    type            INTEGER NOT NULL,
                    active          BOOLEAN NOT NULL DEFAULT FALSE,
                    created_at      BIGINT UNSIGNED NOT NULL,
                    last_seen_at    INTEGER NOT NULL,
                    FOREIGN KEY(user_id) REFERENCES users(id)
                );
            )";
        }


        static void Insert(soci::session& sql, const device& value)
        {
            auto created_at = std::chrono::system_clock::to_time_t(value.created_at);
            auto last_seen_at = std::chrono::system_clock::to_time_t(value.last_seen_at);

            auto active = value.active ? 1 : 0;
            sql <<
                R"(
                    INSERT INTO devices
                    (
                        user_id,
                        device_id,
                        device_name,
                        type,
                        active,
                        created_at,
                        last_seen_at
                    )
                    VALUES
                    (
                        :user_id,
                        :device_id,
                        :device_name,
                        :type,
                        :active,
                        :created_at,
                        :last_seen_at
                    )
                )",
                soci::use(value.user_id),
                soci::use(value.device_id),
                soci::use(value.device_name),
                soci::use(value.type),
                soci::use(active),
                soci::use(created_at),
                soci::use(last_seen_at);
        }


        static device Retrieve(soci::session& sql, uint64_t id)
        {
            device value;
            std::time_t created_at, last_seen_at;
            int active = 0;

            sql <<
                R"(
                    SELECT
                        id,
                        user_id,
                        device_id,
                        device_name,
                        type,
                        active,
                        created_at,
                        last_seen_at
                    FROM devices
                    WHERE id = :id
                )",
                soci::into(value.id),
                soci::into(value.user_id),
                soci::into(value.device_id),
                soci::into(value.device_name),
                soci::into(value.type),
                soci::into(active),
                soci::into(created_at),
                soci::into(last_seen_at),
                soci::use(id);
            
            value.created_at = std::chrono::system_clock::from_time_t(created_at);
            value.last_seen_at = std::chrono::system_clock::from_time_t(last_seen_at);
            value.active = active ? true : false;
            return value;
        }


        static std::vector<device> RetrieveByUserId(soci::session& sql, uint64_t user_id)
        {
            std::vector<device> devices;
            soci::rowset<soci::row> rows =
                (sql.prepare <<
                    R"(
                        SELECT
                            id,
                            user_id,
                            device_id,
                            device_name,
                            type,
                            active,
                            created_at,
                            last_seen_at
                        FROM devices
                        WHERE user_id = :user_id
                    )",
                    soci::use(user_id));

            for (const auto& row : rows)
            {
                device value;
                std::time_t created_at = row.get<std::time_t>(6);
                std::time_t last_seen_at = row.get<std::time_t>(7);

                value.id = row.get<uint64_t>(0);
                value.user_id = row.get<uint64_t>(1);
                value.device_id = row.get<std::string>(2);
                value.device_name = row.get<std::string>(3);
                value.type = row.get<int>(4);
                value.active = row.get<bool>(5);
                value.created_at = std::chrono::system_clock::from_time_t(created_at);
                value.last_seen_at = std::chrono::system_clock::from_time_t(last_seen_at);

                devices.push_back(std::move(value));
            }
            return devices;
        }


        static void Update(soci::session& sql, const device& value)
        {
            auto created_at = std::chrono::system_clock::to_time_t(value.created_at);
            auto last_seen_at = std::chrono::system_clock::to_time_t(value.last_seen_at);

            auto active = value.active ? 1 : 0;
            sql <<
                R"(
                    UPDATE devices
                    SET
                        user_id = :user_id,
                        device_id = :device_id,
                        device_name = :device_name,
                        type = :type,
                        active = :active,
                        created_at = :created_at,
                        last_seen_at = :last_seen_at
                    WHERE id = :id
                )",
                soci::use(value.user_id),
                soci::use(value.device_id),
                soci::use(value.device_name),
                soci::use(value.type),
                soci::use(active),
                soci::use(created_at),
                soci::use(last_seen_at),
                soci::use(value.id);
        }


        static void Delete(soci::session& sql, const device& value)
        {
            sql << "DELETE FROM devices WHERE id = :id", soci::use(value.id);
        }


        static std::vector<device> RetrieveAll(soci::session& sql)
        {
            std::vector<device> devices;
            soci::rowset<soci::row> rows =
                (sql.prepare <<
                    R"(
                        SELECT
                            id,
                            user_id,
                            device_id,
                            device_name,
                            type,
                            active,
                            created_at,
                            last_seen_at
                        FROM devices
                    )");


            for (const auto& row : rows)
            {
                device value;
                std::time_t created_at = row.get<std::time_t>(6);
                std::time_t last_seen_at = row.get<std::time_t>(7);

                value.id = row.get<uint64_t>(0);
                value.user_id = row.get<uint64_t>(1);
                value.device_id = row.get<std::string>(2);
                value.device_name = row.get<std::string>(3);
                value.type = row.get<int>(4);
                value.active = row.get<bool>(5);
                value.created_at = std::chrono::system_clock::from_time_t(created_at);
                value.last_seen_at = std::chrono::system_clock::from_time_t(last_seen_at);

                devices.push_back(std::move(value));
            }
            return devices;
        }
    };
    

} } } }