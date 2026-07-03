#pragma once

#include "_traits.h"
#include <cstdint>
#include <string>
#include <map>
#include <vector>

#include <soci/soci.h>

namespace acl { namespace logos { namespace core { namespace db {

    /* Models */
    enum class WalletType {
        SINGLE_SIG = 1,
        MULTI_SIG = 2,
        TREASURY = 3,
        ESCROW = 4,
        CONTRACT = 5,
    };

    struct WalletKey
    {
        std::string key_id;
        std::string wallet_id;
        std::string public_key;
        uint32_t weight;
        bool active;
    };

    struct Wallet {
        std::string wallet_id;
        std::string identity_id;
        WalletType wallet_type;
        std::vector<WalletKey> keys;
        uint32_t threshold;
        bool frozen;
        bool active;
        std::map<std::string, std::string> metadata;
    };

    struct Transfer {
        std::string transfer_id;
        std::string source_wallet;
        std::string destination_wallet;
        std::string token_id;
        uint64_t amount;
        std::vector<std::string> signatures;            // sigs in string format
        bool executed;
    };
    
    struct TransferSignature
    {
        std::string transfer_id;
        std::string signature;
    };

    /******************************************************** */
    /* TableTraits */

    template<>
    struct TableTraits<WalletKey>
    {
        static std::string CreateTable()
        {
            return R"(
                CREATE TABLE IF NOT EXISTS wallet_keys
                (
                    key_id      VARCHAR(64) PRIMARY KEY,
                    wallet_id   VARCHAR(64) NOT NULL,
                    public_key  TEXT NOT NULL,
                    weight      INTEGER NOT NULL,
                    active      BOOLEAN NOT NULL
                )
            )";
        }

        static void Insert(soci::session& sql, const WalletKey& k)
        {
            const unsigned char active = static_cast<int>(k.active);
            sql << R"(
                INSERT INTO wallet_keys
                (key_id, wallet_id, public_key, weight, active)
                VALUES
                (:key_id, :wallet_id, :public_key, :weight, :active)
            )",
            soci::use(k.key_id),
            soci::use(k.wallet_id),
            soci::use(k.public_key),
            soci::use(k.weight),
            soci::use(active);
        }

        static WalletKey Retrieve(soci::session& sql, const std::string& id)
        {
            WalletKey k;
            unsigned char active = false;

            sql << R"(
                SELECT key_id, wallet_id, public_key, weight, active
                FROM wallet_keys
                WHERE key_id = :id
            )",
            soci::into(k.key_id),
            soci::into(k.wallet_id),
            soci::into(k.public_key),
            soci::into(k.weight),
            soci::into(active),
            soci::use(id);

            k.active = static_cast<bool>(active);

            return k;
        }

        static std::vector<WalletKey> RetrieveAll(soci::session& sql)
        {
            soci::rowset<soci::row> rs =
                (sql.prepare << "SELECT key_id, wallet_id, public_key, weight, active FROM wallet_keys");

            std::vector<WalletKey> out;

            for (const auto& r : rs)
            {
                WalletKey k;
                k.key_id = r.get<std::string>("key_id");
                k.wallet_id = r.get<std::string>("wallet_id");
                k.public_key = r.get<std::string>("public_key");
                k.weight = r.get<int>("weight");
                k.active = r.get<bool>("active");
                out.push_back(k);
            }

            return out;
        }

        static void Update(soci::session& sql, const WalletKey& k)
        {
            const unsigned char active = static_cast<int>(k.active);
            sql << R"(
                UPDATE wallet_keys
                SET public_key = :public_key,
                    weight = :weight,
                    active = :active
                WHERE key_id = :key_id
            )",
            soci::use(k.wallet_id),
            soci::use(k.public_key),
            soci::use(k.weight),
            soci::use(active),
            soci::use(k.key_id);
        }

        static void Delete(soci::session& sql, const WalletKey& k)
        {
            sql << "DELETE FROM wallet_keys WHERE key_id = :id",
            soci::use(k.key_id);
        }
    };



    /************************************************************* */

    template<>
    struct TableTraits<Wallet>
    {
        static std::string CreateTable()
        {
            return R"(
                CREATE TABLE IF NOT EXISTS wallets
                (
                    wallet_id     VARCHAR(64) PRIMARY KEY,
                    identity_id   VARCHAR(64) NOT NULL,
                    wallet_type   INTEGER NOT NULL,
                    threshold     INTEGER NOT NULL,
                    frozen        BOOLEAN NOT NULL,
                    active        BOOLEAN NOT NULL,
                    metadata      TEXT
                )
            )";
        }

        static std::string SerializeMetadata(const std::map<std::string,std::string>& m)
        {
            return "{}"; // replace with JSON later
        }

        static std::map<std::string,std::string> DeserializeMetadata(const std::string&)
        {
            return {};
        }

        static void Insert(soci::session& sql, const Wallet& w)
        {
            int wtype = (int)w.wallet_type;
            unsigned char wfrozen = (unsigned char)w.frozen;
            unsigned char wactive = (unsigned char)w.active;
            std::string emptyMetadata;

            sql << R"(
                INSERT INTO wallets
                (wallet_id, identity_id, wallet_type, threshold, frozen, active, metadata)
                VALUES
                (:wallet_id, :identity_id, :wallet_type, :threshold, :frozen, :active, :metadata)
            )",
            soci::use(w.wallet_id),
            soci::use(w.identity_id),
            soci::use(wtype),
            soci::use(w.threshold),
            soci::use(wfrozen),
            soci::use(wactive),
            soci::use(emptyMetadata /* SerializeMetadata(w.metadata) */ );
        }

        static Wallet Retrieve(soci::session& sql, const std::string& id)
        {
            Wallet w;
            int type;
            unsigned char frozen, active;
            std::string meta;

            sql << R"(
                SELECT wallet_id, identity_id, wallet_type, threshold, frozen, active, metadata
                FROM wallets
                WHERE wallet_id = :id
            )",
            soci::into(w.wallet_id),
            soci::into(w.identity_id),
            soci::into(type),
            soci::into(w.threshold),
            soci::into(frozen),
            soci::into(active),
            soci::into(meta),
            soci::use(id);

            w.wallet_type = static_cast<WalletType>(type);
            w.frozen = static_cast<bool>(frozen);
            w.active = static_cast<bool>(active);
            w.metadata = DeserializeMetadata(meta);

            return w;
        }

        static std::vector<Wallet> RetrieveAll(soci::session& sql)
        {
            soci::rowset<soci::row> rs =
                (sql.prepare << "SELECT wallet_id, identity_id, wallet_type, threshold, frozen, active, metadata FROM wallets");

            std::vector<Wallet> out;

            for (const auto& r : rs)
            {
                Wallet w;
                w.wallet_id = r.get<std::string>("wallet_id");
                w.identity_id = r.get<std::string>("identity_id");
                w.wallet_type = static_cast<WalletType>(r.get<int>("wallet_type"));
                w.threshold = r.get<int>("threshold");
                w.frozen = r.get<bool>("frozen");
                w.active = r.get<bool>("active");
                w.metadata = DeserializeMetadata(r.get<std::string>("metadata"));
                out.push_back(w);
            }

            return out;
        }

        static void Update(soci::session& sql, const Wallet& w)
        {
            int wtype = (int)w.wallet_type;
            int wfrozen = (int)w.frozen;
            int wactive = (int)w.active;

            sql << R"(
                UPDATE wallets
                SET identity_id = :identity_id,
                    wallet_type = :wallet_type,
                    threshold = :threshold,
                    frozen = :frozen,
                    active = :active,
                    -- metadata = :metadata
                WHERE wallet_id = :wallet_id
            )",
            soci::use(w.identity_id),
            soci::use(wtype),
            soci::use(w.threshold),
            soci::use(wfrozen),
            soci::use(wactive),
            //soci::use(SerializeMetadata(w.metadata)),
            soci::use(w.wallet_id);
        }

        static void Delete(soci::session& sql, const Wallet& w)
        {
            sql << "DELETE FROM wallets WHERE wallet_id = :id",
            soci::use(w.wallet_id);
        }
    };


    /**************************************************************** */


    template<>
    struct TableTraits<Transfer>
    {
        static std::string CreateTable()
        {
            return R"(
                CREATE TABLE IF NOT EXISTS transfers
                (
                    transfer_id        VARCHAR(64) PRIMARY KEY,
                    source_wallet      VARCHAR(64) NOT NULL,
                    destination_wallet VARCHAR(64) NOT NULL,
                    token_id           VARCHAR(64) NOT NULL,
                    amount             BIGINT NOT NULL,
                    executed           BOOLEAN NOT NULL
                )
            )";
        }

        static void Insert(soci::session& sql, const Transfer& t)
        {
            int executed = (int)t.executed;

            sql << R"(
                INSERT INTO transfers
                (transfer_id, source_wallet, destination_wallet, token_id, amount, executed)
                VALUES
                (:transfer_id, :source_wallet, :destination_wallet, :token_id, :amount, :executed)
            )",
            soci::use(t.transfer_id),
            soci::use(t.source_wallet),
            soci::use(t.destination_wallet),
            soci::use(t.token_id),
            soci::use(t.amount),
            soci::use(executed);

            // signatures go into separate table
            for (const auto& sig : t.signatures)
            {
                sql << R"(
                    INSERT INTO transfer_signatures
                    (transfer_id, signature)
                    VALUES (:id, :sig)
                )",
                soci::use(t.transfer_id),
                soci::use(sig);
            }
        }

        static Transfer Retrieve(soci::session& sql, const std::string& id)
        {
            Transfer t;
            int executed = false;

            sql << R"(
                SELECT transfer_id, source_wallet, destination_wallet, token_id, amount, executed
                FROM transfers
                WHERE transfer_id = :id
            )",
            soci::into(t.transfer_id),
            soci::into(t.source_wallet),
            soci::into(t.destination_wallet),
            soci::into(t.token_id),
            soci::into(t.amount),
            soci::into(executed),
            soci::use(id);

            t.executed = executed;

            soci::rowset<soci::row> rs =
                (sql.prepare << "SELECT signature FROM transfer_signatures WHERE transfer_id = :id",
                soci::use(id));

            for (const auto& r : rs)
            {
                t.signatures.push_back(r.get<std::string>("signature"));
            }

            return t;
        }

        static std::vector<Transfer> RetrieveAll(soci::session& sql)
        {
            soci::rowset<soci::row> rs =
                (sql.prepare << "SELECT transfer_id FROM transfers");

            std::vector<Transfer> out;

            for (const auto& r : rs)
            {
                out.push_back(Retrieve(sql, r.get<std::string>("transfer_id")));
            }

            return out;
        }

        static void Update(soci::session& sql, const Transfer& t)
        {
            int executed = (int)t.executed;
            sql << R"(
                UPDATE transfers
                SET source_wallet = :source_wallet,
                    destination_wallet = :destination_wallet,
                    token_id = :token_id,
                    amount = :amount,
                    executed = :executed
                WHERE transfer_id = :transfer_id
            )",
            soci::use(t.source_wallet),
            soci::use(t.destination_wallet),
            soci::use(t.token_id),
            soci::use(t.amount),
            soci::use(executed),
            soci::use(t.transfer_id);

            sql << "DELETE FROM transfer_signatures WHERE transfer_id = :id",
            soci::use(t.transfer_id);

            for (const auto& sig : t.signatures)
            {
                sql << R"(
                    INSERT INTO transfer_signatures
                    (transfer_id, signature)
                    VALUES (:id, :sig)
                )",
                soci::use(t.transfer_id),
                soci::use(sig);
            }
        }

        static void Delete(soci::session& sql, const Transfer& t)
        {
            sql << "DELETE FROM transfer_signatures WHERE transfer_id = :id",
            soci::use(t.transfer_id);

            sql << "DELETE FROM transfers WHERE transfer_id = :id",
            soci::use(t.transfer_id);
        }
    };

    /**************************************************************************** */

    template<>
    struct TableTraits<TransferSignature>
    {
        static std::string CreateTable()
        {
            return R"(
                CREATE TABLE IF NOT EXISTS transfer_signatures
                (
                    transfer_id VARCHAR(64) NOT NULL,
                    signature   TEXT NOT NULL
                )
            )";
        }

        static void Insert(soci::session& sql, const TransferSignature& s)
        {
            sql << R"(
                INSERT INTO transfer_signatures
                (transfer_id, signature)
                VALUES (:transfer_id, :signature)
            )",
            soci::use(s.transfer_id),
            soci::use(s.signature);
        }

        static TransferSignature Retrieve(soci::session& sql, const std::string& id)
        {
            TransferSignature s;

            sql << R"(
                SELECT transfer_id, signature
                FROM transfer_signatures
                WHERE transfer_id = :id
                LIMIT 1
            )",
            soci::into(s.transfer_id),
            soci::into(s.signature),
            soci::use(id);

            return s;
        }

        static std::vector<TransferSignature> RetrieveAll(soci::session& sql)
        {
            soci::rowset<soci::row> rs =
                (sql.prepare << "SELECT transfer_id, signature FROM transfer_signatures");

            std::vector<TransferSignature> out;

            for (const auto& r : rs)
            {
                out.push_back({
                    r.get<std::string>("transfer_id"),
                    r.get<std::string>("signature")
                });
            }

            return out;
        }

        static void Update(soci::session& sql, const TransferSignature& s)
        {
            sql << R"(
                UPDATE transfer_signatures
                SET signature = :signature
                WHERE transfer_id = :transfer_id
            )",
            soci::use(s.signature),
            soci::use(s.transfer_id);
        }

        static void Delete(soci::session& sql, const TransferSignature& s)
        {
            sql << R"(
                DELETE FROM transfer_signatures
                WHERE transfer_id = :transfer_id
            )",
            soci::use(s.transfer_id);
        }
    };


} } } }
