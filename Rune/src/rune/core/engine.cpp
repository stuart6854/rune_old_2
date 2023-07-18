#include "rune/core/engine.hpp"

#include "rune/debug/log.hpp"
#include "rune/debug/log_engine.hpp"

namespace rune
{
    bool Engine::run(const EngineConfig& config)
    {
        m_config = config;

        if (!initialise())
        {
            shutdown();
            return false;
        }

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

        return true;
    }

    bool Engine::shutdown()
    {
        RUNE_ENG_INFO("Shutting down...");

        debug::shutdown();

        return false;
    }

}