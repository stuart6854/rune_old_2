#include "core/engine.hpp"
#include "engine_internal.hpp"

#include "core/config_internal.hpp"
#include "platform/platform.hpp"
#include "audio/audio.hpp"
#include "graphics/graphics.hpp"
#include "graphics/renderer/renderer.hpp"
#include "scenes/scenes.hpp"
#include "resources/manager.hpp"
#include "graphics/renderer/static_mesh.hpp"
#include "utility/primitives.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include <memory>
#include <chrono>

namespace rune::engine
{
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

            scenes::new_test_scene();
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
}