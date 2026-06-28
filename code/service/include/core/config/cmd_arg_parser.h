#pragma once

#if __cplusplus >= 202002L
    #include <concepts>
    #include <type_traits>
#endif

#include "core/constants.h"
#include "core/config/cmd_arg_options.h"
#include "core/config/cmd_arg_serializer.h"
#include "core/config/cmd_arg_validator.h"

#include "file.h"   // libcpputils

namespace acl { namespace logos { namespace core {

    #if __cplusplus >= 202002L

    template<typename T>
    concept Serializer = std::derived_from<T, object_serializer<class SettingsObject, Json::Value>>;

    template<typename T>
    concept Validator = std::derived_from<T, object_validator<class SettingsObject>>;

    #endif

    template <class SettingsObject, class SettingsSerializer, class SettingsValidator>
    class CMDArgParser
    {
        public:
            CMDArgParser() {
                #if __cplusplus < 202002L
                    // Pre-C++20
                    static_assert(
                        std::is_base_of_v<object_serializer<class SettingsObject, Json::Value>, SettingsSerializer>,
                        "SettingsSerializer must derive from object_serializer<class SettingsObject, Json::Value>");

                    static_assert(
                        std::is_base_of_v<object_validator<class SettingsObject>, SettingsValidator>,
                        "SettingsValidator must derive from object_validator<class SettingsObject>");
                #endif
            }

            CMDArgParser(const ApplicationInfo& appInfo): _appInfo(appInfo)
            {
                #if __cplusplus < 202002L
                    // Pre-C++20
                    static_assert(
                        std::is_base_of_v<object_serializer<class SettingsObject, Json::Value>, SettingsSerializer>,
                        "SettingsSerializer must derive from object_serializer<class SettingsObject, Json::Value>");

                    static_assert(
                        std::is_base_of_v<object_validator<class SettingsObject>, SettingsValidator>,
                        "SettingsValidator must derive from object_validator<class SettingsObject>");
                #endif
            }


            int parse(int argc, char * argv[])
            {
                int errorCount = 0;
                std::string configFilename;
                for (int i = 1; i < argc; ++i)
                {
                    std::string arg(argv[i]);
                    if (arg == "-h" || arg == "--help")
                    {
                        printUsage();
                        return -1;      // Using -ve return value to indicate args were handled internally within 'parse' method.
                    }

                    if (arg == "-v" || arg == "--version")
                    {
                        printVersion();
                        return -1;
                    }

                    if (arg == "-c" || arg == "--config")
                    {
                        if ((i + 1) >= argc)
                        {
                            std::cerr << "Missing filename after " << arg << std::endl;
                            return 1;
                        }
                        configFilename = argv[++i];
                        continue;
                    }

                    std::cerr << "Unknown argument: " << arg << std::endl;
                    ++errorCount;
                }

                if (configFilename.empty())
                {
                    std::cerr << "No configuration file specified." << std::endl;
                    std::cerr << "Use -c <file>" << std::endl;
                    ++errorCount;
                } else {
                    Json::Value configSettings;
                    if(cpp::utils::read_json_file(configFilename, configSettings))
                    {
                        if(_serializer.convert(configSettings, _settings))
                        {
                            SettingsValidator validator([](const std::string& paramName, const std::string& paramValue, const std::string& errorMsg) {
                                printf("Error validating field '%s' - '%s' is not valid: %s\n", paramName.c_str(), paramValue.c_str(), errorMsg.c_str());
                            });
                            
                            auto validationErrorCount = validator.validate(_settings);
                            if(validationErrorCount == 0)
                            {
                                std::cout << "Successfully validated configuration" << std::endl;
                            } else {
                                std::cerr << "Settings file failed validation with " << validationErrorCount << " errors" << std::endl;
                                errorCount += validationErrorCount;
                                ++errorCount;
                            }
                        } else {
                            std::cerr << "Failed to convert JSON object to settings object" << std::endl;
                            ++errorCount;
                        }
                    } else {
                        std::cerr << "Failed to read JSON file at: " << configFilename << std::endl;
                        ++errorCount;
                    }
                }
                return errorCount;
            }

            const SettingsObject& validatedConfiguration() { return _settings; }


        protected:
            void printVersion()
            {
                std::cout << _appInfo.applicationName << " " << _appInfo.version << '\n';
                std::cout << _appInfo.company << '\n';
                std::cout << _appInfo.copyright << '\n';
            }

            void printUsage()
            {
                printAppInfo();
                std::cout << "\nUsage:\n\n";
                std::cout << "  " << _appInfo.applicationName << " [options]\n\n";
                std::cout << "Options:\n";
                std::cout << "  -h, --help              Display this help\n";
                std::cout << "  -v, --version           Display version information\n";
                std::cout << "  -c, --config <file>     Configuration JSON file\n";
            }

            void printAppInfo()
            {
                std::cout << _appInfo.applicationName << '\n';
                if (!_appInfo.description.empty())
                {
                    std::cout << _appInfo.description << '\n';
                }

                std::cout << '\n';
                std::cout << "Version: " << _appInfo.version << '\n';
                std::cout << _appInfo.company << '\n';
                std::cout << _appInfo.copyright << '\n';
            }

        private:
            SettingsObject _settings;
            SettingsSerializer _serializer;
            ApplicationInfo _appInfo;
    };

    using LogosSvcCMDArgParser = CMDArgParser<LogosSvcSettings, LogosSvcSettingsSerializer, LogosSvcSettingsValidator>;

}}}