#include "core/config/cmd_arg_serializer.h"
#include "string_helpers.h"
#include <cstdlib>

namespace acl { namespace logos { namespace core {

    bool LogosSvcSettingsSerializer::convert(const LogosSvcSettings& settings_obj, Json::Value& settings_json)
    {
        Json::Value logArray;
        for(auto& logSettings : settings_obj.log_settings)
        {
            Json::Value logJson;
            logJson["level"] = logSettings.level;
            logJson["name"] = logSettings.name;
            logJson["pattern"] = logSettings.pattern;
            logJson["output"] = logSettings.output;
            logJson["output-path"] = logSettings.output_path;
            logJson["max-size"] = logSettings.max_size;
            logJson["max-files"] = logSettings.max_files;
            logJson["restart-time"] = logSettings.restart_time;
            logArray.append(logJson);
        }
        settings_json["logging"] = logArray;

        Json::Value identityJson;
        identityJson["node_id"] = settings_obj.identity_settings.node_id;
        identityJson["operator_id"] = settings_obj.identity_settings.operator_id;
        identityJson["network_id"] = settings_obj.identity_settings.network_id;
        identityJson["type"] = settings_obj.identity_settings.keypair.type;
        identityJson["public_key_path"] = settings_obj.identity_settings.keypair.public_key_path;
        identityJson["private_key_path"] = settings_obj.identity_settings.keypair.private_key_path;
        settings_json["identity"] = identityJson;

        Json::Value dbJson, dbCertJson;
        dbJson["engine"] = settings_obj.database_settings.engine;
        dbJson["host"] = settings_obj.database_settings.host;
        dbJson["port"] = settings_obj.database_settings.port;
        dbJson["db"] = settings_obj.database_settings.database;
        dbJson["user"] = settings_obj.database_settings.username;
        dbJson["password"] = settings_obj.database_settings.password;
        dbJson["unix_socket"] = settings_obj.database_settings.unix_socket;
        dbCertJson["use_tls"] = settings_obj.database_settings.certificate.use_tls;
        dbCertJson["cert_path"] = settings_obj.database_settings.certificate.cert_path;
        dbCertJson["key_path"] = settings_obj.database_settings.certificate.key_path;
        dbCertJson["ca_path"] = settings_obj.database_settings.certificate.ca_path;
        dbJson["certificate"] = dbCertJson;
        settings_json["database"] = dbJson;

        Json::Value grpcJson, grpcTlsJson;
        grpcJson["address"] = settings_obj.grpc_settings.address;
        grpcJson["port"] = settings_obj.grpc_settings.port;
        grpcJson["max_thread_count"] = settings_obj.grpc_settings.max_thread_count;
        grpcJson["max_message_size_mb"] = settings_obj.grpc_settings.max_message_size_mb;
        grpcJson["keep_alive_time_ms"] = settings_obj.grpc_settings.keep_alive_time_ms;
        grpcJson["keep_alive_timeout_ms"] = settings_obj.grpc_settings.keep_alive_timeout_ms;
        grpcTlsJson["enabled"] = settings_obj.grpc_settings.tls.use_tls;
        grpcTlsJson["force_client_auth"] = settings_obj.grpc_settings.tls.client_auth;
        grpcTlsJson["cert_path"] = settings_obj.grpc_settings.tls.cert_path;
        grpcTlsJson["key_path"] = settings_obj.grpc_settings.tls.key_path;
        grpcTlsJson["ca_path"] = settings_obj.grpc_settings.tls.ca_path;
        grpcJson["tls"] = grpcTlsJson;
        settings_json["grpc"] = grpcJson;


        return true;
    }
    
    bool LogosSvcSettingsSerializer::convert(const Json::Value& settings_json, LogosSvcSettings& settings_obj)
    {
        // Log settings
        for(auto& logConf: settings_json["logging"])
        {
            LoggerConfig lconf;
            lconf.level = logConf["level"].asString();
            lconf.name = logConf["name"].asString();
            lconf.pattern = logConf["pattern"].asString();
            lconf.output = logConf["output"].asString();
            lconf.output_path = logConf["output-path"].asString();
            lconf.max_size = logConf["max-size"].asInt();
            lconf.max_files = logConf["max-files"].asInt();
            lconf.restart_time = logConf["restart-time"].asString();
            settings_obj.log_settings.push_back(lconf);
        }

        // Identity settings
        auto identity_json = settings_json["identity"];
        auto id_kp_json = identity_json["keypair"];
        auto id_cert_json = identity_json["certificate"];
        settings_obj.identity_settings.node_id = identity_json["node_id"].asString();
        settings_obj.identity_settings.operator_id = identity_json["operator_id"].asString();
        settings_obj.identity_settings.network_id = identity_json["network_id"].asString();
        settings_obj.identity_settings.keypair.type = id_kp_json["type"].asString();
        settings_obj.identity_settings.keypair.public_key_path = id_kp_json["public_key_path"].asString();
        settings_obj.identity_settings.keypair.private_key_path = id_kp_json["private_key_path"].asString();

        /*
        if(id_cert_json["use_tls"].isBool())
            settings_obj.identity_settings.certificate.use_tls = id_cert_json["use_tls"].asBool();
        else
            settings_obj.identity_settings.certificate._use_tls = id_cert_json["use_tls"].asString();

        settings_obj.identity_settings.certificate.ca_path = id_cert_json["ca_path"].asString();
        settings_obj.identity_settings.certificate.cert_path = id_cert_json["cert_path"].asString();
        settings_obj.identity_settings.certificate.key_path = id_cert_json["key_path"].asString();
        */

        // Database settings
        auto database_json = settings_json["database"];
        settings_obj.database_settings.engine = database_json["engine"].asString();

        auto engine_json = database_json[settings_obj.database_settings.engine];
        settings_obj.database_settings.host = engine_json["host"].asString();
        settings_obj.database_settings.unix_socket = engine_json["unix_socket"].asString();

        if(engine_json["port"].isInt())
            settings_obj.database_settings.port = engine_json["port"].asInt();
        else
            settings_obj.database_settings._port = engine_json["port"].asString();

        settings_obj.database_settings.database = engine_json["db"].asString();
        settings_obj.database_settings.username = engine_json["user"].asString();
        settings_obj.database_settings.password = engine_json["password"].asString();

        if(engine_json["certificate"]["use_tls"].isBool())
            settings_obj.database_settings.certificate.use_tls = engine_json["certificate"]["use_tls"].asBool();
        else
            settings_obj.database_settings.certificate._use_tls = engine_json["certificate"]["use_tls"].asString();

        settings_obj.database_settings.certificate.cert_path = engine_json["certificate"]["cert_path"].asString();
        settings_obj.database_settings.certificate.key_path = engine_json["certificate"]["key_path"].asString();
        settings_obj.database_settings.certificate.ca_path = engine_json["certificate"]["ca_path"].asString();

        // grpc settings
        auto grpc_json = settings_json["grpc"];
        auto grpc_tls_json = grpc_json["tls"];

        if(grpc_json["max_thread_count"].isInt()) {
            int tc = grpc_json["max_thread_count"].asInt();
            settings_obj.grpc_settings.max_thread_count = (tc <= 0 ? 10 : tc);
        } else {
            settings_obj.grpc_settings.max_thread_count = 0;
            settings_obj.grpc_settings._max_thread_count = grpc_json["max_thread_count"].asString();
        }

        settings_obj.grpc_settings.address = grpc_json["address"].asString();
        
        if(grpc_json["port"].isInt())
            settings_obj.grpc_settings.port = grpc_json["port"].asInt();
        else
            settings_obj.grpc_settings._port = grpc_json["port"].asString();

        if(grpc_json["max_thread_count"].isInt())
            settings_obj.grpc_settings.max_thread_count = grpc_json["max_thread_count"].asInt();
        else
            settings_obj.grpc_settings._max_thread_count = grpc_json["max_thread_count"].asString();

        if(grpc_json["max_message_size_mb"].isInt())
            settings_obj.grpc_settings.max_message_size_mb = grpc_json["max_message_size_mb"].asInt();
        else
            settings_obj.grpc_settings._max_message_size_mb = grpc_json["max_message_size_mb"].asString();
        
        settings_obj.grpc_settings.keep_alive_time_ms = grpc_json["keep_alive_time_ms"].asInt();
        settings_obj.grpc_settings.keep_alive_timeout_ms = grpc_json["keep_alive_timeout_ms"].asInt();

        if(grpc_tls_json["enabled"].isBool())
            settings_obj.grpc_settings.tls.use_tls = grpc_tls_json["enabled"].asBool();
        else
            settings_obj.grpc_settings.tls._use_tls = grpc_tls_json["enabled"].asString();

        settings_obj.grpc_settings.tls.client_auth = grpc_tls_json["force_client_auth"].asBool();
        settings_obj.grpc_settings.tls.cert_path = grpc_tls_json["cert_path"].asString();
        settings_obj.grpc_settings.tls.key_path = grpc_tls_json["key_path"].asString();
        settings_obj.grpc_settings.tls.ca_path = grpc_tls_json["ca_path"].asString();

        return true;
    }


}}}