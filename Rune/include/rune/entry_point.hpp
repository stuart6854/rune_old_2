#pragma once

#include <string_view>

namespace rune
{
    struct EngineConfig
    {
        std::string_view logDirPath;
        std::string_view configFilename;
        std::string_view initialScene;
    };

    extern void configure_engine(EngineConfig& config);
}