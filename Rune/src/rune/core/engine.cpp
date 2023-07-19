#include "rune/core/engine.hpp"

#include "rune/debug/log.hpp"
#include "rune/debug/log_engine.hpp"
#include "rune/platform/platform.hpp"

namespace rune
{
    bool Engine::run(const EngineConfig& config)
    {
        m_config = config;

        if (!initialise())
        {
            RUNE_ENG_CRITICAL("Failed to initialise. Will now shutdown!");
            shutdown();
            return false;
        }

        main_loop();

        if (!shutdown())
        {
            return false;
        }

        return true;
    }

    bool Engine::initialise()
    {
        debug::initialise(m_config.logOutputDir);
        RUNE_ENG_INFO("Rune Engine v0.0.1");
        RUNE_ENG_INFO("Initialising...");

        if (!platform::initialise(m_events))
            return false;

        return true;
    }

    bool Engine::shutdown()
    {
        RUNE_ENG_INFO("Shutting down...");

        platform::shutdown();
        debug::shutdown();

        return true;
    }

    void Engine::main_loop()
    {
        int x = 10;
        while (x > 0)
        {
            platform::update();
            m_events.update();

            --x;
        }
    }

}