#include "internal_engine.hpp"
#include "entry_point.hpp"

#include "core/config_internal.hpp"
#include "core/config.hpp"
#include "platform/platform.hpp"
#include "audio/audio.hpp"

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
    }

    void shutdown()
    {
        auto& engineData = get_engine_data();
        RUNE_UNUSED(engineData);

        LOG_INFO("Rune shutting down...");
        audio::shutdown();
        platform::shutdown();
        config::internal::write_config(engineData.config.configFilename);

        s_engineData = nullptr;
    }

    void run()
    {
        auto* primaryWindow = platform::create_window(800, 600, "Primary Window");
        platform::show_window(primaryWindow);

        auto lastTime = platform::get_time();
        while (!platform::has_window_requested_close(primaryWindow))
        {
            auto time = platform::get_time();
            const auto deltaTime = time - lastTime;
            lastTime = time;

            platform::update();

            platform::set_window_title(primaryWindow, std::format("Primary Window - {:.2f}ms", deltaTime));

            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }

        platform::destroy_window(primaryWindow);
    }

}