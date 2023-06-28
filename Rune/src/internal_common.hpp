#pragma once

#include "common.hpp"
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

#define RUNE_THROW_EX(_msg) throw ::rune::RuneException(_msg, __LINE__, __FILE__)

#define RUNE_UNUSED(_x) (void)(_x)

#define COMBINE1(_x, _y) _x##_y  // Helper macros
#define COMBINE(_x, _y) COMBINE1(_x, _y)

#include <string>
#include <format>
#include <exception>
#include <string_view>

namespace rune
{
    class RuneException : public std::exception
    {
    public:
        RuneException(std::string_view msg, std::uint32_t line, std::string_view file)
        {
            m_what = std::format("Rune exception - {}\nFile: {}\n Line: {}", msg, file, line);
        }

        const char* what() const override { return m_what.c_str(); }

    private:
        std::string m_what{};
    };
}