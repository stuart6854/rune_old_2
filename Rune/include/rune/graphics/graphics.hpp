#pragma once

#include "common.hpp"

#include <gfx/gfx.hpp>

using namespace sm;

namespace rune::graphics
{
    void initialise(void* platformWindowHandle);
    void shutdown();
}