#pragma once

#include "common.hpp"
#include "core/platform_detection.hpp"
#include "core/engine.hpp"
#include "core/system_logging.hpp"

#include <tracy/Tracy.hpp>

#include <source_location>

#define GLFW_NATIVE_INCLUDE_NONE
#if defined(RUNE_PLATFORM_WINDOWS)
    #define GLFW_EXPOSE_NATIVE_WIN32
    #define NOMINMAX
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

#if defined(RUNE_BUILD_DEBUG) || defined(RUNE_BUILD_RELEASE)
    #define LOG_TRACE(...) \
        ::rune::Engine::get().get_system<SystemLogging>()->log(std::source_location::current(), LogLevel::Trace, __VA_ARGS__)
    #define LOG_DEBUG(...) \
        ::rune::Engine::get().get_system<SystemLogging>()->log(std::source_location::current(), LogLevel::Debug, __VA_ARGS__)
    #define LOG_INFO(...) \
        ::rune::Engine::get().get_system<SystemLogging>()->log(std::source_location::current(), LogLevel::Info, __VA_ARGS__)
    #define LOG_WARN(...) \
        ::rune::Engine::get().get_system<SystemLogging>()->log(std::source_location::current(), LogLevel::Warn, __VA_ARGS__)
    #define LOG_ERROR(...) \
        ::rune::Engine::get().get_system<SystemLogging>()->log(std::source_location::current(), LogLevel::Error, __VA_ARGS__)
    #define LOG_CRITICAL(...) \
        ::rune::Engine::get().get_system<SystemLogging>()->log(std::source_location::current(), LogLevel::Critical, __VA_ARGS__)

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

    #define RUNE_ASSERT(_expr)                                                                             \
        do                                                                                                 \
        {                                                                                                  \
            if (!(_expr))                                                                                  \
            {                                                                                              \
                auto sourceLoc = std::source_location::current();                                          \
                LOG_ERROR("{}:{}: assertion failed: {}", sourceLoc.file_name(), sourceLoc.line(), #_expr); \
                RUNE_DEBUG_BREAK();                                                                        \
            }                                                                                              \
        } while (false)

    #define RUNE_PROFILE_SCOPE() ZoneScoped
    #define RUNE_PROFILE_FRAME() FrameMark
    #define RUNE_PROFILE_SECTION(_x) ZoneScopedN(_x)
    #define RUNE_PROFILE_TAG(_x) ZoneText(_x, strlen(_x))               // Give regions more detail
    #define RUNE_PROFILE_LOG(_text, _size) TracyMessage(_text, _size)   // Send logs to Tracy app (view logs alongside profile data)
    #define RUNE_PROFILE_VALUE(_text, _value) TracyPlot(_text, _value)  // Plot numbers

#elif defined(RUNE_BUILD_DISTRO)
    #define LOG_TRACE(...)
    #define LOG_DEBUG(...)
    #define LOG_INFO(...)
    #define LOG_WARN(...)
    #define LOG_ERROR(...)
    #define LOG_CRITICAL(...)

    #define RUNE_DEBUG_BREAK()
    #define RUNE_ASSERT(_expr)

    #define RUNE_PROFILE_SCOPE()
    #define RUNE_PROFILE_FRAME()
    #define RUNE_PROFILE_SECTION(_x)
    #define RUNE_PROFILE_TAG(_x)
    #define RUNE_PROFILE_LOG(_text, _size)
    #define RUNE_PROFILE_VALUE(_text, _value)
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

        const char* what() const noexcept override { return m_what.c_str(); }

    private:
        std::string m_what{};
    };
}