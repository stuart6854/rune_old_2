#pragma once

#include "internal_common.hpp"

#include <string_view>

namespace rune::config::internal
{
    void load_config(std::string_view configFilename);
    void write_config(std::string_view configFilename);
}