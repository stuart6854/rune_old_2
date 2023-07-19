#pragma once

#include "rune/events/events.hpp"

#include <string>

// TODO: Decide if we will implement game code DLLs - http://glampert.com/2014/01-03/game-dll/

namespace rune
{
    struct EngineConfig
    {
        std::string logOutputDir;
    };

    class IApplication;

    class Engine
    {
    public:
        bool run(IApplication* app, const EngineConfig& config);

        void request_shutdown();

        auto get_events() -> Events& { return m_events; }

    private:
        bool initialise();
        bool shutdown();

        void main_loop();

    private:
        IApplication* m_application{ nullptr };
        EngineConfig m_config{};
        Events m_events{};

        bool m_isRunning{ false };
    };
}