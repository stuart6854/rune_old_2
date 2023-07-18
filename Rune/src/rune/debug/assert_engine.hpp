#pragma once

#include "rune/common.hpp"
#include "rune/debug/log_engine.hpp"

#define RUNE_ENG_ASSERT(_x)                                         \
    do                                                              \
    {                                                               \
        if (!(_x))                                                  \
        {                                                           \
            RUNE_ENG_ERROR("Assertion failed: {} == {}", ##_x, _x); \
            RUNE_DEBUG_BREAK();                                     \
        }                                                           \
    } while (false)