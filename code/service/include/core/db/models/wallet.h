#pragma once

#include "_traits.h"
#include <cstdint>
#include <string>
#include <map>
#include <vector>

namespace acl { namespace logos { namespace core { namespace db {

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
        std::vector<uint8_t> public_key;
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
                    public_key  BLOB NOT NULL,
                    weight      INTEGER NOT NULL,
                    active      BOOLEAN NOT NULL
                )
            )";
        }
    };

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
    };

    template<>
    struct TableTraits<Transfer>
    {
        static std::string CreateTable()
        {
            return R"(
                CREATE TABLE IF NOT EXISTS transfers
                (
                    transfer_id          VARCHAR(64) PRIMARY KEY,
                    source_wallet        VARCHAR(64) NOT NULL,
                    destination_wallet   VARCHAR(64) NOT NULL,
                    token_id             VARCHAR(64) NOT NULL,
                    amount               BIGINT NOT NULL,
                    executed             BOOLEAN NOT NULL
                )
            )";
        }
    };

    template<>
    struct TableTraits<TransferSignature>
    {
        static std::string CreateTable()
        {
            return R"(
                CREATE TABLE IF NOT EXISTS transfer_signatures
                (
                    transfer_id   VARCHAR(64) NOT NULL,
                    signature     TEXT NOT NULL
                )
            )";
        }
    };

} } } }
