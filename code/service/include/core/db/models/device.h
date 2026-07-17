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
        bool active;

        std::chrono::system_clock::time_point created_at;
    };

    template<>
    struct TableTraits<device>
    {
        static std::string CreateTable()
        {
            return R"(
                CREATE TABLE IF NOT EXISTS devices
                (
                    id          INTEGER PRIMARY KEY AUTOINCREMENT,
                    user_id     INTEGER NOT NULL,
                    device_id   TEXT NOT NULL,
                    active      BOOLEAN NOT NULL DEFAULT FALSE,
                    created_at  INTEGER NOT NULL,
                    FOREIGN KEY(user_id) REFERENCES users(id)
                );
            )";
        }


        static void Insert(soci::session& sql, const device& value)
        {
            auto created_at = std::chrono::system_clock::to_time_t(value.created_at);
            auto active = value.active ? 1 : 0;
            sql <<
                R"(
                    INSERT INTO devices
                    (
                        user_id,
                        device_id,
                        active,
                        created_at
                    )
                    VALUES
                    (
                        :user_id,
                        :device_id,
                        :active,
                        :created_at
                    )
                )",
                soci::use(value.user_id),
                soci::use(value.device_id),
                soci::use(active),
                soci::use(created_at);
        }


        static device Retrieve(soci::session& sql, uint64_t id)
        {
            device value;
            std::time_t created_at;
            int active = 0;

            sql <<
                R"(
                    SELECT
                        id,
                        user_id,
                        device_id,
                        active,
                        created_at
                    FROM devices
                    WHERE id = :id
                )",
                soci::into(value.id),
                soci::into(value.user_id),
                soci::into(value.device_id),
                soci::into(active),
                soci::into(created_at),
                soci::use(id);
            
            value.created_at = std::chrono::system_clock::from_time_t(created_at);
            value.active = active ? true : false;
            return value;
        }


        static void Update(soci::session& sql, const device& value)
        {
            auto created_at = std::chrono::system_clock::to_time_t(value.created_at);
            auto active = value.active ? 1 : 0;
            sql <<
                R"(
                    UPDATE devices
                    SET
                        user_id = :user_id,
                        device_id = :device_id,
                        active = :active,
                        created_at = :created_at
                    WHERE id = :id
                )",
                soci::use(value.user_id),
                soci::use(value.device_id),
                soci::use(active),
                soci::use(created_at),
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
                            active,
                            created_at
                        FROM devices
                    )");


            for (const auto& row : rows)
            {
                device value;
                std::time_t created_at = row.get<std::time_t>(4);

                value.id = row.get<uint64_t>(0);
                value.user_id = row.get<uint64_t>(1);
                value.device_id = row.get<std::string>(2);
                value.active = row.get<bool>(3);
                value.created_at = std::chrono::system_clock::from_time_t(created_at);

                devices.push_back(std::move(value));
            }
            return devices;
        }
    };
    

} } } }