
#include "core/config/cmd_arg_placeholder.h"
#include <cstdlib>
#include <regex>
#include <stdexcept>
#include <string>

namespace acl { namespace logos { namespace core {

    bool SettingsPlaceholder::ResolveEnvironmentVariables(const std::string& input, std::string& output)
    {
        static const std::regex envRegex(R"(\$\{([A-Za-z_][A-Za-z0-9_]*)\})");
        std::string result;
        std::sregex_iterator begin(input.begin(), input.end(), envRegex);
        std::sregex_iterator end;
        std::size_t lastPos = 0;

        for (auto it = begin; it != end; ++it)
        {
            const std::smatch& match = *it;

            // Copy everything before the match
            result.append(input, lastPos, match.position() - lastPos);
            const std::string envName = match[1].str();
            if (const char* value = std::getenv(envName.c_str()))
            {
                result += value;
            }
            else
            {
                // TODO: Is this dealt with
                throw std::runtime_error("Environment variable '" + envName + "' is not defined.");
            }
            lastPos = match.position() + match.length();
        }

        // Copy any remaining text
        result.append(input, lastPos, std::string::npos);
        output = result;
        return !output.empty();
    }

    void SettingsPlaceholder::resolve_placeholders(const LogosSvcSettings& input, LogosSvcSettings& output)
    {
        std::string resolved;
        if(ResolveEnvironmentVariables("${DATABASE_HOST}", resolved))
            output.database_settings.host = resolved;
        
        if(!output.database_settings._port.empty() && ResolveEnvironmentVariables("${DATABASE_PORT}", resolved))
        {
            output.database_settings._port = resolved;
            output.database_settings.port = atoi(resolved.c_str());
        }
        
        if(ResolveEnvironmentVariables("${DATABASE_NAME}", resolved))
            output.database_settings.database = resolved;
        
        if(ResolveEnvironmentVariables("${DATABASE_USER}", resolved))
            output.database_settings.username = resolved;
        
        if(ResolveEnvironmentVariables("${DATABASE_PASS}", resolved))
            output.database_settings.password = resolved;

        if(ResolveEnvironmentVariables("${DATABASE_UNIX_SOCKET}", resolved))
            output.database_settings.unix_socket = resolved;

        
        if(!output.database_settings.certificate._use_tls.empty() && ResolveEnvironmentVariables("${DATABASE_TLS}", resolved))
        {
            output.database_settings.certificate._use_tls = resolved;
            output.database_settings.certificate.use_tls = atoi(resolved.c_str()) > 0 ? true : false;
        }


        if(ResolveEnvironmentVariables("${DATABASE_CERT}", resolved))
            output.database_settings.certificate.cert_path = resolved;

        if(ResolveEnvironmentVariables("${DATABASE_KEY}", resolved))
            output.database_settings.certificate.key_path = resolved;
        
        if(ResolveEnvironmentVariables("${DATABASE_CA}", resolved))
            output.database_settings.certificate.ca_path = resolved;
        
        if(ResolveEnvironmentVariables("${DATABASE_UNIX_SOCKET}", resolved))
            output.database_settings.unix_socket = resolved;

        
        
        if(ResolveEnvironmentVariables("${GRPC_ADDRESS}", resolved))
            output.grpc_settings.address = resolved;
        
        if(!output.grpc_settings._port.empty() && ResolveEnvironmentVariables("${GRPC_PORT}", resolved))
        {
            output.grpc_settings._port = resolved;
            output.grpc_settings.port = atoi(resolved.c_str());
        }
        
        if(!output.grpc_settings._max_thread_count.empty() && ResolveEnvironmentVariables("${GRPC_MAX_THREADS}", resolved))
        {
            output.grpc_settings._max_thread_count = resolved;
            output.grpc_settings.max_thread_count = atoi(resolved.c_str());
        }
        
        if(!output.grpc_settings.tls._use_tls.empty() && ResolveEnvironmentVariables("${GRPC_MAX_MESSAGE_SIZE_MB}", resolved))
        {
            output.grpc_settings.tls._use_tls = resolved;
            output.grpc_settings.tls.use_tls = atoi(resolved.c_str()) > 0 ? true : false;
        }
        
        if(!output.grpc_settings.tls._use_tls.empty() && ResolveEnvironmentVariables("${GRPC_TLS}", resolved))
        {
            output.grpc_settings.tls._use_tls = resolved;
            output.grpc_settings.tls.use_tls = atoi(resolved.c_str()) > 0 ? true : false;
        }

        if(ResolveEnvironmentVariables("${GRPC_CERT}", resolved))
            output.grpc_settings.tls.cert_path = resolved;
        
        if(ResolveEnvironmentVariables("${GRPC_KEY}", resolved))
            output.grpc_settings.tls.key_path = resolved;
        
        if(ResolveEnvironmentVariables("${GRPC_CA}", resolved))
            output.grpc_settings.tls.ca_path = resolved;
        



        if(ResolveEnvironmentVariables("${IDENTITY_NODE_ID}", resolved))
            output.identity_settings.node_id = resolved;
        
        if(ResolveEnvironmentVariables("${IDENTITY_OPERATOR_ID}", resolved))
            output.identity_settings.operator_id = resolved;
        
        if(ResolveEnvironmentVariables("${IDENTITY_NETWORK_ID}", resolved))
            output.identity_settings.network_id = resolved;
        
        if(ResolveEnvironmentVariables("${IDENTITY_KEYTYPE}", resolved))
            output.identity_settings.keypair.type = resolved;
        
        if(ResolveEnvironmentVariables("${IDENTITY_PUBKEY}", resolved))
            output.identity_settings.keypair.public_key_path = resolved;
        
        if(ResolveEnvironmentVariables("${IDENTITY_PRIVKEY}", resolved))
            output.identity_settings.keypair.private_key_path = resolved;
    }

} } }