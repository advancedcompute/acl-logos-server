#pragma once

#include <functional>
#include <string>

#include "core/config/cmd_arg_options.h"
#include "core/interfaces/object_validator.h"

namespace acl { namespace logos { namespace core {


    class LogosSvcSettingsValidator: public object_validator<LogosSvcSettings>
    {
        public:
            explicit LogosSvcSettingsValidator(std::function<void(std::string, std::string, std::string)> callback):
                object_validator<LogosSvcSettings>(callback) {}

            int validate(LogosSvcSettings& config) override;

    };


}}}