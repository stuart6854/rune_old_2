#pragma once

#include "common.hpp"
#include "entry_point.hpp"

namespace rune::engine::internal
{
    struct EngineData
    {
        EngineConfig config{};
        bool isRunning{ false };
    };

    auto get_engine_data() -> EngineData&;

    void initialise(const EngineConfig& config);
    void shutdown();

}
