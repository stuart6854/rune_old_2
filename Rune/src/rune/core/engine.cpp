#include "rune/core/engine.hpp"

#include "rune/debug/log.hpp"
#include "rune/debug/log_engine.hpp"
#include "rune/core/app.hpp"
#include "rune/platform/platform.hpp"

namespace rune
{
    bool Engine::run(IApplication* app, const EngineConfig& config)
    {
        m_application = app;
        m_config = config;

        if (!initialise())
        {
            RUNE_ENG_CRITICAL("Failed to initialise. Will now shutdown!");
            shutdown();
            return false;
        }

        m_isRunning = true;
        main_loop();

        if (!shutdown())
        {
            return false;
        }

        return true;
    }

    void Engine::request_shutdown()
    {
        m_isRunning = false;
    }

    bool Engine::initialise()
    {
        debug::initialise(m_config.logOutputDir);
        RUNE_ENG_INFO("Rune Engine v0.0.1");
        RUNE_ENG_INFO("Initialising...");

        if (!platform::initialise(m_events))
            return false;

        if (m_application)
            m_application->initialise(*this);

        return true;
    }

    bool Engine::shutdown()
    {
        RUNE_ENG_INFO("Shutting down...");

        if (m_application)
            m_application->shutdown(*this);

        platform::shutdown();
        debug::shutdown();

        return true;
    }

    void Engine::main_loop()
    {
        while (m_isRunning)
        {
            platform::update();
            m_events.update();

            if (m_application)
                m_application->update(*this);
        }
    }

}