#pragma once

#include <string>
#include <vector>

namespace acl { namespace logos { namespace core {

    struct ApplicationInfo
    {
        std::string applicationName;
        std::string company;
        std::string version;
        std::string copyright;
        std::string description;

        ApplicationInfo(std::string name, std::string comp, std::string vers, std::string cpyrgt, std::string desc):
            applicationName(name),
            company(comp),
            version(vers),
            copyright(cpyrgt),
            description(desc)
        {}
    };

    struct Certificate
    {
        bool use_tls;
        std::string cert_path;
        std::string key_path;
        std::string ca_path;
    };

    struct KeyPair
    {
        std::string type;
        std::string private_key_path;
        std::string public_key_path;
    };

    struct IdentityConfig
    {
        public:
            std::string node_id;
            std::string operator_id;
            KeyPair     keypair;
            Certificate certificate;


    };

    struct LoggerConfig
    {
        public:
            std::string name;
            std::string level;
            std::string output;
            std::string output_path;
            std::string pattern;
            int    max_size = 1073741824;      // Default to 1gb. Fairly large but OK.
            int    max_files = 10;
            std::string restart_time;
    };

    struct DBEngine
    {
        public:
            std::string host;
            int port;
    };

    struct GrpcConfig
    {
        public:
            std::string address;
            int port;
            int max_thread_count = 10;
            int max_message_size_mb;
            int keep_alive_time_ms;
            int keep_alive_timeout_ms;
            Certificate tls;
    };

    struct DatabaseConfig
    {
        public:
            std::string engine;
            std::string host;
            std::string database;
            std::string username;
            std::string password;
            int port;
            Certificate certificate;
    };

    class LogosSvcSettings
    {
        public:
            
            IdentityConfig  identity_settings;
            std::vector<LoggerConfig> log_settings;
            DatabaseConfig  database_settings;
            GrpcConfig      grpc_settings;
    };



}}}