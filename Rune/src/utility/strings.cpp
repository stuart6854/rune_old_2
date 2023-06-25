#include "utility/strings.hpp"

#include <ranges>

namespace rune::utility::strings
{
    void left_trim(std::string& str)
    {
        str.erase(str.begin(), std::ranges::find_if(str, [](unsigned char c) { return !std::isspace(c); }));
    }

    void right_trim(std::string& str)
    {
        str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char c) { return !std::isspace(c); }).base(), str.end());
    }

    void trim(std::string& str)
    {
        left_trim(str);
        right_trim(str);
    }

    auto left_trim_copy(std::string str) -> std::string
    {
        left_trim(str);
        return str;
    }

    auto right_trim_copy(std::string str) -> std::string
    {
        right_trim(str);
        return str;
    }

    auto trim_copy(std::string str) -> std::string
    {
        left_trim(str);
        right_trim(str);
        return str;
    }
}