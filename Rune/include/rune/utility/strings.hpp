#pragma once

#include <string>

namespace rune::utility::strings
{
    void left_trim(std::string& str);
    void right_trim(std::string& str);
    void trim(std::string& str);

    auto left_trim_copy(std::string str) -> std::string;
    auto right_trim_copy(std::string str) -> std::string;
    auto trim_copy(std::string str) -> std::string;
}