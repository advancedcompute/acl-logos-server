
#include "core/config/cmd_arg_placeholder.h"
#include <cstdlib>
#include <regex>
#include <stdexcept>
#include <string>

namespace acl { namespace logos { namespace core {

    std::string SettingsPlaceholder::ResolveEnvironmentVariables(const std::string& input)
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
                throw std::runtime_error("Environment variable '" + envName + "' is not defined.");
            }
            lastPos = match.position() + match.length();
        }

        // Copy any remaining text
        result.append(input, lastPos, std::string::npos);
        return result;
    }

    void SettingsPlaceholder::resolve_placeholders(const LogosSvcSettings& input, LogosSvcSettings& output)
    {
        auto resolved = ResolveEnvironmentVariables("${DATABASE_HOST}");

        printf("Resolved: %s\n", resolved.c_str());
        
    }

} } }