#pragma once

#include "rune/common.hpp"
#include "rune/debug/log.hpp"

#define RUNE_CLIENT_ASSERT(_x)                                         \
    do                                                                 \
    {                                                                  \
        if (!(_x))                                                     \
        {                                                              \
            RUNE_CLIENT_ERROR("Assertion failed: {} == {}", ##_x, _x); \
            RUNE_DEBUG_BREAK();                                        \
        }                                                              \
    } while (false)