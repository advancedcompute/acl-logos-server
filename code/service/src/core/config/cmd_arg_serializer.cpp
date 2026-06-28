#include "core/config/cmd_arg_serializer.h"

#include <iostream>

namespace acl { namespace logos { namespace core {

    bool LogosSvcSettingsSerializer::convert(const LogosSvcSettings& settings_obj, Json::Value& settings_json)
    {
        // TODO
        return false;
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
        settings_obj.identity_settings.keypair.type = id_kp_json["type"].asString();
        settings_obj.identity_settings.keypair.public_key_path = id_kp_json["public_key_path"].asString();
        settings_obj.identity_settings.keypair.private_key_path = id_kp_json["private_key_path"].asString();

        settings_obj.identity_settings.certificate.use_tls = id_cert_json["use_tls"].asBool();
        settings_obj.identity_settings.certificate.ca_path = id_cert_json["ca_path"].asString();
        settings_obj.identity_settings.certificate.cert_path = id_cert_json["cert_path"].asString();
        settings_obj.identity_settings.certificate.key_path = id_cert_json["key_path"].asString();

        // Database settings
        auto database_json = settings_json["database"];
        settings_obj.database_settings.engine = database_json["engine"].asString();
        auto engine_json = database_json[settings_obj.database_settings.engine];
        settings_obj.database_settings.host = engine_json["host"].asString();
        settings_obj.database_settings.port = engine_json["port"].asInt();
        settings_obj.database_settings.database = engine_json["db"].asString();
        settings_obj.database_settings.username = engine_json["user"].asString();
        settings_obj.database_settings.password = engine_json["password"].asString();
        settings_obj.database_settings.certificate.use_tls = engine_json["certificate"]["use_tls"].asBool();
        settings_obj.database_settings.certificate.cert_path = engine_json["certificate"]["cert_path"].asString();
        settings_obj.database_settings.certificate.key_path = engine_json["certificate"]["key_path"].asString();
        settings_obj.database_settings.certificate.ca_path = engine_json["certificate"]["ca_path"].asString();



        return true;
    }


}}}