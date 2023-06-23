#include "core/engine.hpp"

#include "common.hpp"
#include "internal/internal_engine.hpp"

namespace rune::engine
{
    void request_shutdown()
    {
        auto& engineData = internal::get_engine_data();
        engineData.isRunning = false;
    }
}