#include "internal_engine.hpp"
#include "entry_point.hpp"

#include "core/config_internal.hpp"
#include "core/config.hpp"
#include "platform/platform.hpp"

#include <memory>

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
    }

    void shutdown()
    {
        auto& engineData = get_engine_data();
        RUNE_UNUSED(engineData);

        LOG_INFO("Rune shutting down...");
        platform::shutdown();
        config::internal::write_config(engineData.config.configFilename);

        s_engineData = nullptr;
    }

}