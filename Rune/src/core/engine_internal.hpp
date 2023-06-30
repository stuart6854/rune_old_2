#pragma once

#include "internal_common.hpp"
#include "entry_point.hpp"
#include "platform/platform.hpp"

namespace rune::engine::internal
{
    struct EngineData
    {
        EngineConfig config{};
        bool isRunning{ false };
        platform::WindowHandle primaryWindow{ nullptr };
        std::vector<platform::WindowHandle> windows{};

        f32 deltaTime{ 0.0f };
    };

    auto get_engine_data() -> EngineData&;

    void initialise(const EngineConfig& config);
    void shutdown();

    void run();

}
