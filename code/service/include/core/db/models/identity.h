#pragma once

#include "_traits.h"
#include <cstdint>
#include <string>
#include <map>
#include <vector>

namespace acl { namespace logos { namespace core { namespace db {

    enum class IdentityType
    {
        DEVICE = 0,
        PERSON = 1,
        COMPANY = 2,
        VALIDATOR = 3,
        SERVICE = 4,
        ORACLE = 5,
        DAO = 6,
    };

    struct Identity
    {
        std::string identity_id;
        IdentityType identity_type;
        std::string first_name;
        std::string middle_names;
        std::string last_name;
        std::vector<uint8_t> public_key;
        bool verified;
        bool active;
        uint64_t created_at;
        std::map<std::string, std::string> metadata;
    };

    template<>
    struct TableTraits<Identity>
    {
        static constexpr std::string_view Name = "identities";

        static std::string CreateTable()
        {
            return R"(
                CREATE TABLE IF NOT EXISTS identities
                (
                    identity_id    VARCHAR(64) PRIMARY KEY,
                    identity_type  INTEGER NOT NULL,
                    first_name     TEXT,
                    middle_names   TEXT,
                    last_name      TEXT,
                    public_key     BLOB NOT NULL,
                    verified       BOOLEAN NOT NULL,
                    active         BOOLEAN NOT NULL,
                    created_at     BIGINT NOT NULL,
                    metadata       JSON
                )
            )";
        }
    };


}}}}