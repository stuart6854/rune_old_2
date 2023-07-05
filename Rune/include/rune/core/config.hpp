#pragma once

#include "common_internal.hpp"

#include <string>
#include <unordered_map>

namespace rune::config
{
    auto get_bool(const std::string& key, bool defaultValue) -> bool;
    auto get_int(const std::string& key, int defaultValue) -> int;
    auto get_float(const std::string& key, f32 defaultValue) -> f32;
    auto get_string(const std::string& key, const std::string& defaultValue) -> const std::string&;
}