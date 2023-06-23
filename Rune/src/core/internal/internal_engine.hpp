#pragma once

#include "common.hpp"

namespace rune::engine::internal
{
    struct EngineData
    {
        bool isRunning{ false };
    };

    auto get_engine_data() -> EngineData&;

    void initialise();
    void shutdown();

}
