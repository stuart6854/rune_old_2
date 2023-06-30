#pragma once

#include "common.hpp"
#include "platform/platform.hpp"

namespace rune::engine
{
    auto get_primary_window() -> platform::WindowHandle;

    auto get_delta_time() -> f32;

    void request_shutdown();
}
