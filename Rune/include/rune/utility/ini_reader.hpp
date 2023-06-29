#pragma once

#include <string>
#include <vector>
#include <optional>
#include <string_view>
#include <unordered_map>

namespace rune::utility
{
    class INIReader
    {
    public:
        INIReader() = default;
        ~INIReader() = default;

        void parse(std::string_view filename);

        auto get_errors() const -> std::string_view;

        auto get_sections() const -> std::vector<std::string_view>;
        auto get_keys(std::string_view section) const -> std::optional<std::vector<std::string_view>>;
        auto get_value(std::string_view section, std::string_view key) -> std::optional<std::string_view>;

    private:
        void parse_content(std::string_view content);

    private:
        using ValueMap = std::unordered_map<std::string, std::string>;
        std::unordered_map<std::string, ValueMap> m_sectionMap;

        std::string m_errors{};
    };
}