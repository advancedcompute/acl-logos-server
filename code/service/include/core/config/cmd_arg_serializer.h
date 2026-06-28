#pragma once

#include <json/json.h>
#include "core/config/cmd_arg_options.h"
#include "core/interfaces/object_serializer.h"

namespace acl { namespace logos { namespace core {


    class LogosSvcSettingsSerializer: public object_serializer<LogosSvcSettings, Json::Value>
    {
    public:
        bool convert(const LogosSvcSettings& u, Json::Value& v) override;
        bool convert(const Json::Value& v, LogosSvcSettings& u) override;


    };

}}}