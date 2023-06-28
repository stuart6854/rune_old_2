#include "internal_engine.hpp"
#include "entry_point.hpp"

#include "core/config_internal.hpp"
#include "core/config.hpp"
#include "platform/platform.hpp"
#include "audio/audio.hpp"
#include "graphics/graphics.hpp"
#include "graphics/renderer.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include <memory>
#include <chrono>

namespace rune::engine::internal
{
    namespace
    {
        std::unique_ptr<EngineData> s_engineData{ nullptr };  // NOLINT
    }

    auto get_engine_data() -> EngineData&
    {
        RUNE_ASSERT(s_engineData != nullptr);
        return *s_engineData;
    }

    void initialise(const EngineConfig& config)
    {
        RUNE_ASSERT(s_engineData == nullptr);
        s_engineData = std::make_unique<EngineData>();

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
    }

    void shutdown()
    {
        auto& engineData = get_engine_data();
        RUNE_UNUSED(engineData);

        LOG_INFO("Rune shutting down...");
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

        graphics::Renderer renderer{};
        renderer.initialise();

        auto lastTime = platform::get_time();
        while (!platform::has_window_requested_close(engineData.primaryWindow))
        {
            auto time = platform::get_time();
            const auto deltaTime = time - lastTime;
            lastTime = time;

            platform::update();

            platform::set_window_title(engineData.primaryWindow, std::format("Primary Window - {:.2f}ms", deltaTime));

            auto windowSize = platform::get_window_size_pixels(engineData.primaryWindow);
            renderer.render_camera({
                engineData.primaryWindow,
                windowSize,
                glm::perspectiveLH_ZO(glm::radians(70.0f), f32(windowSize.x) / f32(windowSize.y), 0.1f, 100.0f),
                glm::lookAtLH(glm::vec3(-1.0f, 1.5f, -2.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
            });
#if 0
            for (auto* window : engineData.windows)
            {
                renderer.render_camera({ window, platform::get_window_size_pixels(window) });
            }
#endif

            renderer.render_static_mesh(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)));
            renderer.render_static_mesh(glm::translate(glm::mat4(1.0f), glm::vec3(2, 0, 0)));
            renderer.render_static_mesh(glm::translate(glm::mat4(1.0f), glm::vec3(0, 2, 0)));
            renderer.render_static_mesh(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 2)));

            renderer.flush_renders();
            //            graphics::render_temp();

            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }

        renderer.shutdown();
    }

}