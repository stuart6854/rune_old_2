// #TODO: Default system priority when system is provided (if priority is not already set)

#include "core/engine.hpp"

#include "core/config_internal.hpp"
#include "core/system_logging.hpp"
#include "events/system_events.hpp"
#include "platform/system_platform_glfw.hpp"
#include "rendering/system_renderer.hpp"
#include "scripting/system_scripting.hpp"
#include "resources/system_resources.hpp"
#include "scenes/system_scene.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include <memory>
#include <chrono>
#include <ranges>
#include <thread>

namespace rune
{
#if 0
    std::unique_ptr<internal::EngineData> s_engineData{ nullptr };  // NOLINT

    auto get_primary_window() -> platform::WindowHandle
    {
        const auto& engineData = internal::get_engine_data();
        return engineData.primaryWindow;
    }

    auto get_delta_time() -> f32
    {
        const auto& engineData = internal::get_engine_data();
        return engineData.deltaTime;
    }

    void request_shutdown()
    {
        LOG_INFO("engine - Shutdown requested");
        auto& engineData = internal::get_engine_data();
        engineData.isRunning = false;
    }

    namespace internal
    {
        auto get_engine_data() -> internal::EngineData&
        {
            RUNE_ASSERT(s_engineData != nullptr);
            return *s_engineData;
        }

        void initialise(const EngineConfig& config)
        {
            RUNE_ASSERT(s_engineData == nullptr);
            s_engineData = std::make_unique<internal::EngineData>();

            auto& engineData = get_engine_data();
            engineData.config = config;

            LOG_INFO("Rune initialising...");
            config::internal::load_config(engineData.config.configFilename);
            platform::initialise();
            audio::initialise();

    #if 0
        for (auto i = 0; i < 5; ++i)
        {
            engineData.windows.push_back(platform::create_window(400, 400, "Window " + std::to_string(i)));
            platform::show_window(engineData.windows.back());
        }
    #endif
            engineData.primaryWindow = platform::create_window(800, 600, "Primary Window");
            platform::show_window(engineData.primaryWindow);

            graphics::initialise();
            graphics::renderer::initialise();

            scenes::initialise();

            resources::initialise();

            resources::add_resource(STRID("builtin_unit_cube"), utility::primitives::generate_cube(1.0f, { 0.0f, 0.0f, 0.0f }));
            resources::add_resource(STRID("builtin_plane_5m"), utility::primitives::generate_plane({ 5, 5 }, { -2.5f, -2.5f }));

            scenes::load_scene("scene_01.yaml");
            //            scenes::new_test_scene();
        }

        void shutdown()
        {
            auto& engineData = get_engine_data();
            RUNE_UNUSED(engineData);

            LOG_INFO("Rune shutting down...");

            resources::shutdown();
            scenes::shutdown();

            graphics::renderer::shutdown();
            graphics::shutdown();
            audio::shutdown();

            for (auto* window : engineData.windows)
            {
                platform::destroy_window(window);
            }
            platform::destroy_window(engineData.primaryWindow);

            platform::shutdown();
            config::internal::write_config(engineData.config.configFilename);

            s_engineData = nullptr;
        }

        void run()
        {
            auto& engineData = get_engine_data();
            engineData.isRunning = true;

            auto lastTime = platform::get_time();
            while (engineData.isRunning)
            {
                auto time = platform::get_time();
                engineData.deltaTime = f32(time - lastTime);
                lastTime = time;

                platform::update();
                if (platform::has_window_requested_close(engineData.primaryWindow))
                {
                    request_shutdown();
                }

                platform::set_window_title(engineData.primaryWindow, std::format("Primary Window - {:.2f}ms", engineData.deltaTime));

                scenes::update();

    #if 0
                auto windowSize = platform::get_window_size_pixels(engineData.primaryWindow);
                graphics::renderer::render_camera({
                    engineData.primaryWindow,
                    windowSize,
                    glm::perspectiveLH_ZO(glm::radians(70.0f), f32(windowSize.x) / f32(windowSize.y), 0.1f, 100.0f),
                    glm::lookAtLH(camPosition, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
                });
                for (auto* window : engineData.windows)
                {
                    renderer.render_camera({ window, platform::get_window_size_pixels(window) });
                }
    #endif

                graphics::renderer::flush_renders();
                //            graphics::render_temp();

                std::this_thread::sleep_for(std::chrono::milliseconds(16));
            }
        }

    }
#endif

    static Engine* s_engine{ nullptr };  // NOLINT

    auto Engine::get() -> Engine&
    {
        RUNE_ASSERT(s_engine != nullptr);
        return *s_engine;
    }

    Engine::Engine(const EngineConfig& config) : m_config(config)
    {
        RUNE_ASSERT(s_engine == nullptr);
        s_engine = this;
    }

    Engine::~Engine()
    {
        s_engine = nullptr;
    }

    void Engine::run()
    {
        initialise();

        m_isRunning = true;

        auto lastTime = get_system<SystemPlatform>()->get_time();
        while (m_isRunning)
        {
            RUNE_PROFILE_FRAME();

            auto time = get_system<SystemPlatform>()->get_time();
            m_deltaTime = f32(time - lastTime);
            lastTime = time;

            update();

            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }

        shutdown();
    }

    void Engine::request_shutdown()
    {
        m_isRunning = false;
    }

    void Engine::initialise()
    {
        RUNE_PROFILE_SCOPE();

        set_system_priority<SystemLogging>(100);
        provide_system<SystemLogging>(std::make_unique<SystemLogging>(m_config.logDirPath));

        set_system_priority<SystemEvents>(90);
        provide_system<SystemEvents>(std::make_unique<SystemEvents>());

        set_system_priority<SystemPlatform>(80);
        provide_system<SystemPlatform>(std::make_unique<SystemPlatformGLFW>());

        set_system_priority<SystemRenderer>(50);
        provide_system<SystemRenderer>(std::make_unique<SystemRenderer>());

        set_system_priority<SystemScripting>(40);
        provide_system<SystemScripting>(std::make_unique<SystemScripting>());

        set_system_priority<SystemResources>(20);
        provide_system<SystemResources>(std::make_unique<SystemResources>());

        set_system_priority<SystemScene>(10);
        provide_system<SystemScene>(std::make_unique<SystemScene>());

        for (auto [system, _] : m_systemPriorityVec)
        {
            m_systemMap.at(system)->initialise();
        }
        LOG_INFO("All systems initialised.");

#ifdef TRACY_ENABLE
        LOG_INFO("Profiling enabled.");
#endif

        m_primaryWindow = get_system<SystemPlatform>()->create_window(1280, 720, "Rune");
        get_system<SystemPlatform>()->show_window(m_primaryWindow);

        if (!m_config.initialScene.empty())
        {
            get_system<SystemScene>()->load_scene(m_config.initialScene);
        }
    }

    void Engine::update()
    {
        RUNE_PROFILE_SCOPE();

        for (auto [system, _] : m_systemPriorityVec)
        {
            m_systemMap.at(system)->update();
        }

        if (get_system<SystemPlatform>()->has_window_requested_close(m_primaryWindow))
        {
            request_shutdown();
        }

        get_system<SystemPlatform>()->set_window_title(m_primaryWindow,
                                                       std::format("Primary Window - {:.2f}s/{}ms", m_deltaTime, u32(m_deltaTime * 1000)));

        get_system<SystemRenderer>()->flush();
    }

    void Engine::shutdown()
    {
        LOG_INFO("Shutting all systems down.");

        std::ranges::reverse_view systemPriorityReverse{ m_systemPriorityVec };
        for (auto [system, _] : systemPriorityReverse)
        {
            m_systemMap.at(system)->shutdown();
        }
    }

}