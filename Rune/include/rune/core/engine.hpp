#pragma once

#include "rune/events/events.hpp"

#include <string>

namespace rune
{
    struct EngineConfig
    {
        std::string logOutputDir;
    };

    class Engine
    {
    public:
        bool run(const EngineConfig& config);

        auto get_events() -> Events& { return m_events; }

    private:
        bool initialise();
        bool shutdown();

        void main_loop();

    private:
        EngineConfig m_config{};
        Events m_events{};
    };
}