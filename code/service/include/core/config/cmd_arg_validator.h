#pragma once

#include <functional>
#include <string>

#include "ecc.h"
#include "ed25519.h"
#include "rsa.h"

#include "core/config/cmd_arg_options.h"
#include "core/interfaces/object_validator.h"

namespace acl { namespace logos { namespace core {


    class LogosSvcSettingsValidator: public object_validator<LogosSvcSettings>
    {
        public:
            explicit LogosSvcSettingsValidator(std::function<void(std::string, std::string, std::string)> callback):
                object_validator<LogosSvcSettings>(callback) {}

            int validate(LogosSvcSettings& config) override;

        private:

            int validateLogSettings(LogosSvcSettings& config);

            int validateDatabaseSettings(LogosSvcSettings& config);

            int validateIdentitySettings(LogosSvcSettings& config);

            int validateGrpcSettings(LogosSvcSettings& config);


            int validateECCIdentityKey(LogosSvcSettings& config, cpp::utils::ECC& key);
            int validateEDIdentityKey(LogosSvcSettings& config, cpp::utils::ED25519& key);
            int validateRSAIdentityKey(LogosSvcSettings& config, cpp::utils::RSA& key);
            
            //int generateIdKeys(LogosSvcSettings& config);

            std::string         id_key_type = "ecc";
            cpp::utils::ECC     identity_ecc_key;
            cpp::utils::ED25519 identity_ed_key;
            cpp::utils::RSA     identity_rsa_key;

            std::vector<std::string> permittedKeyTypes = {
                "rsa", "ecc", "ed25519"
            };

    };


}}}