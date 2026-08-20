#pragma once

#include <soci/soci.h>
#include "_traits.h"
#include <string>
#include <vector>
#include <chrono>
#include "device.h"


namespace acl { namespace logos { namespace core { namespace db {

    struct user
    {
        uint64_t id;
        std::string first_name;
        std::string last_name;
        std::string display_name;
        std::string phone_number;
        std::string email_address;
        bool email_verified;
        bool phone_verified;
        std::chrono::system_clock::time_point created_at;
    };

    template<>
    struct TableTraits<user>
    {

        static std::string CreateTable()
        {
            return R"(
                CREATE TABLE IF NOT EXISTS users
                (
                    id              INTEGER AUTO_INCREMENT PRIMARY KEY,
                    first_name      TEXT NOT NULL,
                    last_name       TEXT NOT NULL,
                    display_name    TEXT NOT NULL,
                    phone_number    TEXT,
                    email_address   TEXT,
                    email_verified  BOOLEAN NOT NULL DEFAULT FALSE,
                    phone_verified  BOOLEAN NOT NULL DEFAULT FALSE,
                    created_at      BIGINT UNSIGNED NOT NULL
                );
            )";
        }

        static void Insert(soci::session& sql, const user& value)
        {
            auto timestamp = std::chrono::system_clock::to_time_t(value.created_at);
            auto everified = value.email_verified ? 1 : 0;
            auto pverified = value.phone_verified ? 1 : 0;

            sql <<
                R"(
                    INSERT INTO users
                    (
                        first_name,
                        last_name,
                        display_name,
                        phone_number,
                        email_address,
                        email_verified,
                        phone_verified,
                        created_at
                    )
                    VALUES
                    (
                        :first_name,
                        :last_name,
                        :display_name,
                        :phone_number,
                        :email_address,
                        :email_verified,
                        :phone_verified,
                        :created_at
                    )
                )",
                soci::use(value.first_name),
                soci::use(value.last_name),
                soci::use(value.display_name),
                soci::use(value.phone_number),
                soci::use(value.email_address),
                soci::use(everified),
                soci::use(pverified),
                soci::use(timestamp);
        }

        static user Retrieve(soci::session& sql, uint64_t id)
        {
            user value;
            std::time_t timestamp;
            uint8_t e_verified = 0, p_verified = 0;
            soci::indicator ind;

            sql <<
                R"(
                    SELECT
                        id,
                        first_name,
                        last_name,
                        display_name,
                        phone_number,
                        email_address,
                        email_verified,
                        phone_verified,
                        created_at
                    FROM users
                    WHERE id = :id
                )",
                soci::into(value.id, ind),
                soci::into(value.first_name),
                soci::into(value.last_name),
                soci::into(value.display_name),
                soci::into(value.phone_number),
                soci::into(value.email_address),
                soci::into(e_verified),
                soci::into(p_verified),
                soci::into(timestamp),
                soci::use(id);
            
            if (ind == soci::i_null)
            {
                throw soci::soci_error("User not found");
            }
            
            value.email_verified = e_verified ? true : false;
            value.phone_verified = p_verified ? true : false;
            value.created_at = std::chrono::system_clock::from_time_t(timestamp);
            return value;
        }

        static void Update(soci::session& sql, const user& value)
        {
            auto everified = value.email_verified ? 1 : 0;
            auto pverified = value.phone_verified ? 1 : 0;
            
            sql <<
                R"(
                    UPDATE users
                    SET
                        first_name = :first_name,
                        last_name = :last_name,
                        display_name = :display_name,
                        phone_number = :phone_number,
                        email_address = :email_address,
                        email_verified = :email_verified,
                        phone_verified = :phone_verified
                    WHERE id = :id
                )",
                soci::use(value.first_name),
                soci::use(value.last_name),
                soci::use(value.display_name),
                soci::use(value.phone_number),
                soci::use(value.email_address),
                soci::use(everified),
                soci::use(pverified),
                soci::use(value.id);
        }

        static void Delete(soci::session& sql, const user& identity)
        {
            sql << "DELETE FROM users WHERE id = :id", soci::use(identity.id);
        }

        static std::vector<user> RetrieveAll(soci::session& sql)
        {
            std::vector<user> users;
            user value;
            soci::rowset<soci::row> rows =
                (sql.prepare <<
                    R"(
                        SELECT
                            id,
                            first_name,
                            last_name,
                            display_name,
                            phone_number,
                            email_address,
                            email_verified,
                            phone_verified,
                            created_at
                        FROM users
                    )");

            for (const auto& row : rows)
            {
                user item;
                std::time_t timestamp;

                item.id              = row.get<uint64_t>(0);
                item.first_name      = row.get<std::string>(1);
                item.last_name       = row.get<std::string>(2);
                item.display_name    = row.get<std::string>(3);
                item.phone_number    = row.get<std::string>(4);
                item.email_address   = row.get<std::string>(5);
                item.email_verified  = row.get<bool>(6);
                item.phone_verified  = row.get<bool>(7);

                timestamp            = row.get<std::time_t>(8);
                item.created_at      = std::chrono::system_clock::from_time_t(timestamp);

                users.push_back(std::move(item));
            }
            return users;
        }

    };

} } } }