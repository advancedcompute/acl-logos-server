#pragma once
#include "core/config/cmd_arg_options.h"

namespace acl { namespace logos { namespace core {

    class SettingsPlaceholder
    {
    public:
        void resolve_placeholders(const LogosSvcSettings& input, LogosSvcSettings& output);
    
    private:
        bool ResolveEnvironmentVariables(const std::string& input, std::string& output);
    };

} } }