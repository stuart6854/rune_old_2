#include "utility/ini_reader.hpp"

#include "common.hpp"
#include "utility/io.hpp"
#include "utility/strings.hpp"

#include <format>
#include <sstream>

namespace rune::utility
{
    void INIReader::parse(std::string_view filename)
    {
        m_sectionMap.clear();
        m_errors.clear();

        auto contentResult = io::read_string(filename);
        if (!contentResult)
        {
            m_errors = std::format("Failed to read INI file: {}", filename);
            return;
        }
        const auto& content = contentResult.value();

        parse_content(content);
    }

    auto INIReader::get_errors() const -> std::string_view
    {
        return m_errors;
    }

    auto INIReader::get_sections() const -> std::vector<std::string_view>
    {
        std::vector<std::string_view> sections{};
        sections.reserve(m_sectionMap.size());
        for (const auto& [section, _] : m_sectionMap)
        {
            sections.push_back(section);
        }
        return sections;
    }

    auto INIReader::get_keys(std::string_view section) const -> std::optional<std::vector<std::string_view>>
    {
        const auto it = m_sectionMap.find(std::string(section));
        if (it == m_sectionMap.end())
        {
            return std::nullopt;
        }
        const auto& valueMap = it->second;

        std::vector<std::string_view> keys{};
        keys.reserve(valueMap.size());
        for (const auto& [key, _] : valueMap)
        {
            keys.push_back(key);
        }
        return keys;
    }

    auto INIReader::get_value(std::string_view section, std::string_view key) -> std::optional<std::string_view>
    {
        const auto sectionIt = m_sectionMap.find(std::string(section));
        if (sectionIt == m_sectionMap.end())
        {
            return std::nullopt;
        }
        const auto& valueMap = sectionIt->second;

        const auto valueIt = valueMap.find(std::string(key));
        if (valueIt == valueMap.end())
        {
            return std::nullopt;
        }
        const auto& value = valueIt->second;
        return value;
    }

    void INIReader::parse_content(std::string_view content)
    {
        std::uint32_t lineIndex{};
        std::string section;

        std::stringstream ss(content.data());
        std::string line{};
        while (std::getline(ss, line))
        {
            ++lineIndex;

            utility::strings::trim(line);
            if (line.empty() || line.starts_with(';'))
            {
                continue;
            }
            if (line.starts_with('['))
            {
                if (!line.ends_with(']'))
                {
                    m_errors += std::format("line {}: line does not end with ']'.\n", lineIndex);
                    continue;
                }
                section = line.substr(1, line.size() - 2);
                continue;
            }
            if (line.contains('='))
            {
                const auto delimiterIndex = line.find('=');
                auto key = std::string(line.substr(0, delimiterIndex));
                utility::strings::trim(key);

                auto value = std::string(line.substr(delimiterIndex + 1, line.size() - delimiterIndex));
                utility::strings::trim(value);
                m_sectionMap[section][key] = value;
                continue;
            }

            m_errors += std::format("line {}: invalid line.\n", lineIndex);
        }
    }

}