#include "internal_engine.hpp"

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

    void initialise()
    {
        RUNE_ASSERT(s_engineData == nullptr);
        s_engineData = std::make_unique<EngineData>();

        LOG_INFO("Rune initialising...");
    }

    void shutdown()
    {
        auto& engineData = get_engine_data();
        RUNE_UNUSED(engineData);

        LOG_INFO("Rune shutting down...");

        s_engineData = nullptr;
    }

}