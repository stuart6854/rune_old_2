#pragma once

#include "core/platform_detection.hpp"
#include "core/internal/internal_logging.hpp"

#include <source_location>

#if defined(RUNE_BUILD_DEBUG) || defined(RUNE_BUILD_RELEASE)
    #if defined(RUNE_PLATFORM_WINDOWS)
        #define RUNE_DEBUG_BREAK() __debugbreak()
    #elif defined(RUNE_PLATFORM_LINUX)
        #include <signal.h>
        #if defined(SIGTRAP)
            #define RUNE_DEBUG_BREAK() raise(SIGTRAP)
        #else
            #define RUNE_DEBUG_BREAK() raise(SIGABRT)
        #endif
    #endif

    #define RUNE_ASSERT(_expr)                                                                                    \
        do                                                                                                        \
        {                                                                                                         \
            if (!(_expr))                                                                                         \
            {                                                                                                     \
                std::source_location sourceLoc{};                                                                 \
                LOG_ERROR("{}:{}: error: assertion failed: {}", sourceLoc.file_name(), sourceLoc.line(), #_expr); \
                RUNE_DEBUG_BREAK();                                                                               \
            }                                                                                                     \
        } while (false)

#elif defined(RUNE_BUILD_DISTRO)
    #define RUNE_DEBUG_BREAK()
    #define RUNE_ASSERT(_expr)
#else
    #error Rune build type has not been defined!
#endif

#define RUNE_UNUSED(_x) (void)(_x)

namespace rune
{

}