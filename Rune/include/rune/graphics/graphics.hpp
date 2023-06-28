#pragma once

#include "common.hpp"

#include <gfx/gfx.hpp>

using namespace sm;

namespace rune::graphics
{
    void initialise();
    void shutdown();

    auto get_device() -> gfx::DeviceHandle;
    void render_temp();
}