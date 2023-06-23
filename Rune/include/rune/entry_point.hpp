#pragma once

#include <string_view>

namespace rune
{
    struct EngineConfig
    {
        std::string_view logDirPath;
    };

    extern void configure_engine(EngineConfig& config);
}