#pragma once

#include <soci/soci.h>
#include "_traits.h"
#include <cstdint>
#include <string>
#include <map>
#include <vector>
#include <json/json.h>
//#include "file.h"

namespace acl { namespace logos { namespace core { namespace db {

    enum class IdentityType: int
    {
        PERSON = 0,
        COMPANY = 1,
        VALIDATOR = 2,
        SERVICE = 3,
        ORACLE = 4,
        DAO = 5,
    };

    struct Identity
    {
        std::string identity_id;
        IdentityType identity_type;
        std::string public_key;
        bool verified = false;
        bool active = false;
        uint64_t created_at;
        std::map<std::string, std::string> metadata;
    };

    template<>
    struct TableTraits<Identity>
    {
        static constexpr const char* TableName = "identities";
        static constexpr const char* PrimaryKey = "identity_id";

        static std::string CreateTable()
        {
            return R"(
                CREATE TABLE IF NOT EXISTS identities
                (
                    identity_id     VARCHAR(64) PRIMARY KEY,
                    identity_type   INTEGER NOT NULL,
                    public_key      TEXT NOT NULL,
                    verified        BOOLEAN NOT NULL,
                    active          BOOLEAN NOT NULL,
                    created_at      BIGINT NOT NULL,
                    metadata        TEXT
                )
            )";
        }

        static void Insert(soci::session& sql, const Identity& identity)
        {
            const int type = static_cast<int>(identity.identity_type);
            const std::string metadata = SerializeMetadata(identity.metadata);
            const int verified = static_cast<int>(identity.verified);
            const int active = static_cast<int>(identity.active);

            sql << R"(
                INSERT INTO identities
                (
                    identity_id,
                    identity_type,
                    public_key,
                    verified,
                    active,
                    created_at,
                    metadata
                )
                VALUES
                (
                    :identity_id,
                    :identity_type,
                    :public_key,
                    :verified,
                    :active,
                    :created_at,
                    :metadata
                )
            )",
            soci::use(identity.identity_id),
            soci::use(type),
            soci::use(identity.public_key),
            soci::use(verified),
            soci::use(active),
            soci::use(identity.created_at),
            soci::use(metadata);
        }

        static Identity Retrieve(soci::session& sql, const std::string& identityId)
        {
            Identity identity;

            int type;
            std::string metadata;
            int verified=0, active=0;

            sql << R"(
                SELECT
                    identity_id,
                    identity_type,
                    public_key,
                    verified,
                    active,
                    created_at,
                    metadata
                FROM identities
                WHERE identity_id = :identity_id
            )",
            soci::into(identity.identity_id),
            soci::into(type),
            soci::into(identity.public_key),
            soci::into(verified),
            soci::into(active),
            soci::into(identity.created_at),
            soci::into(metadata),
            soci::use(identityId);

            identity.identity_type = static_cast<IdentityType>(type);
            identity.verified = static_cast<bool>(verified);
            identity.active = static_cast<bool>(active);

            identity.metadata = DeserializeMetadata(metadata);
            return identity;
        }

        static void Update(soci::session& sql, const Identity& identity)
        {
            const int type = static_cast<int>(identity.identity_type);
            const std::string metadata = SerializeMetadata(identity.metadata);
            const int verified = static_cast<int>(identity.verified);
            const int active = static_cast<int>(identity.active);

            sql << R"(
                UPDATE identities
                SET
                    identity_type = :identity_type,
                    public_key = :public_key,
                    verified = :verified,
                    active = :active,
                    created_at = :created_at,
                    metadata = :metadata
                WHERE
                    identity_id = :identity_id
            )",
            soci::use(type),
            soci::use(identity.public_key),
            soci::use(verified),
            soci::use(active),
            soci::use(identity.created_at),
            soci::use(metadata),
            soci::use(identity.identity_id);
        }

        static void Delete(soci::session& sql, const Identity& identity)
        {
            sql << R"(
                DELETE FROM identities
                WHERE identity_id = :identity_id
            )",
            soci::use(identity.identity_id);
        }

        static std::vector<Identity> RetrieveAll(soci::session& sql)
        {
            soci::rowset<soci::row> rs = (sql.prepare << "SELECT * FROM identities");

            std::vector<Identity> identities;

            for (const auto& row : rs)
            {
                //int verified=0, active=0;
                Identity identity;
                identity.identity_id = row.get<std::string>("identity_id");
                identity.identity_type = static_cast<IdentityType>(row.get<int>("identity_type"));
                identity.public_key = row.get<std::string>("public_key");
                identity.verified = row.get<int>("verified");
                identity.active = row.get<int>("active");
                identity.created_at = row.get<uint64_t>("created_at");

                identity.metadata = DeserializeMetadata(row.get<std::string>("metadata"));
                identities.push_back(std::move(identity));
            }
            return identities;
        }

        static Json::Value JsonMetadata(const std::map<std::string, std::string>& metadata)
        {
            Json::Value jsonObj;
            for(auto& md : metadata) {
                jsonObj[md.first] = md.second;
            }
            return jsonObj;
        }

        static Json::Value JsonMetadata(const std::string& metadata)
        {
            Json::CharReaderBuilder rbuilder;
            Json::CharReader * reader = rbuilder.newCharReader();
            std::string parseErrors;        // Note: this var is thrown away
            Json::Value contents;

            if( !reader->parse(metadata.data(), metadata.data() + metadata.size(), &contents, &parseErrors) )
            {
                // TODO
            }
            return contents;
        }
        
        static std::string SerializeMetadata(const std::map<std::string, std::string>& metadata)
        {
            Json::Value jsonObj = JsonMetadata(metadata);

            std::stringstream ss;
            try {
                // Configure writer for pretty output
                Json::StreamWriterBuilder writerBuilder;
                writerBuilder["indentation"] = "  "; // 2 spaces for readability
                std::unique_ptr<Json::StreamWriter> writer(writerBuilder.newStreamWriter());
                writer->write(jsonObj, &ss);
            } catch (const std::exception& e) {
                // TODO: Handle this better
            }
            return ss.str();
        }

        static std::map<std::string, std::string> DeserializeMetadata(Json::Value& metadata)
        {
            std::map<std::string, std::string> retMap;
            for(const std::string& member : metadata.getMemberNames())
            {
                retMap[member] = metadata[member].asString();
            }
            return retMap;
        }

        static std::map<std::string, std::string> DeserializeMetadata(const std::string& metadata)
        {
            std::map<std::string, std::string> retMap;
            Json::Value contents = JsonMetadata(metadata);

            for(auto& member : contents.getMemberNames())
            {
                retMap[member] = contents[member].asString();
            }
            return retMap;
        }


    };


}}}}