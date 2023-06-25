#include "core/config.hpp"
#include "config_internal.hpp"

#include "utility/ini_reader.hpp"

#include <string>
#include <unordered_map>

namespace rune::config
{
    namespace
    {
        std::unordered_map<std::string, std::string> g_configValues{};  // NOLINT
    }

    namespace internal
    {
        void load_config(std::string_view configFilename)
        {
            utility::INIReader reader{};
            reader.parse(configFilename);

            if (!reader.get_errors().empty())
            {
                LOG_ERROR(reader.get_errors());
            }

            const auto sections = reader.get_sections();
            for (const auto& section : sections)
            {
                const auto keys = reader.get_keys(section).value_or(std::vector<std::string_view>());
                for (const auto& key : keys)
                {
                    std::string configKey{};
                    if (section.empty())
                    {
                        configKey = key;
                    }
                    else
                    {
                        configKey = std::format("{}.{}", section, key);
                    }

                    const auto& value = reader.get_value(section, key).value_or("");
                    g_configValues[configKey] = value;
                }
            }
        }

        void write_config(std::string_view /*configFilename*/) {}
    }

    auto get_bool(const std::string& key, bool defaultValue) -> bool
    {
        auto defaultValueStr = std::to_string(defaultValue);
        const auto& stringValue = get_string(key, defaultValueStr);
        return std::stoi(stringValue);
    }

    auto get_int(const std::string& key, int defaultValue) -> std::int32_t
    {
        auto defaultValueStr = std::to_string(defaultValue);
        const auto& stringValue = get_string(key, defaultValueStr);
        return std::stoi(stringValue);
    }

    auto get_float(const std::string& key, float defaultValue) -> float
    {
        auto defaultValueStr = std::to_string(defaultValue);
        const auto& stringValue = get_string(key, defaultValueStr);
        return std::stof(stringValue);
    }

    auto get_string(const std::string& key, const std::string& defaultValue) -> const std::string&
    {
        const auto it = g_configValues.find(key);
        if (it != g_configValues.end())
        {
            return it->second;
        }
        return defaultValue;
    }

}