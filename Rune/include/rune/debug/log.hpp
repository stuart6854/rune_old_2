#pragma once

#include "rune/common.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <format>
#include <string_view>

namespace rune::debug
{
    enum class LogLevel : u8
    {
        Trace = 0,
        Debug,
        Info,
        Warn,
        Error,
        Critical,
        Off,
    };

    void initialise(const std::string& logOutputDir);
    void shutdown();

    void log_client(const char* file, i32 line, LogLevel level, std::string_view msg);

    template <typename... Args>
    void log_client(const char* file, i32 line, LogLevel level, std::string_view fmt, Args&&... args)
    {
        log_client(file, line, level, std::vformat(fmt, std::make_format_args(args...)));
    }
}

#define RUNE_CLIENT_TRACE(...) ::rune::debug::log_engine(__FILE__, __LINE__, ::rune::debug::LogLevel::Trace, __VA_ARGS__)
#define RUNE_CLIENT_DEBUG(...) ::rune::debug::log_engine(__FILE__, __LINE__, ::rune::debug::LogLevel::Debug, __VA_ARGS__)
#define RUNE_CLIENT_INFO(...) ::rune::debug::log_engine(__FILE__, __LINE__, ::rune::debug::LogLevel::Info, __VA_ARGS__)
#define RUNE_CLIENT_WARN(...) ::rune::debug::log_engine(__FILE__, __LINE__, ::rune::debug::LogLevel::Warn, __VA_ARGS__)
#define RUNE_CLIENT_ERROR(...) ::rune::debug::log_engine(__FILE__, __LINE__, ::rune::debug::LogLevel::Error, __VA_ARGS__)
#define RUNE_CLIENT_CRITICAL(...) ::rune::debug::log_engine(__FILE__, __LINE__, ::rune::debug::LogLevel::Critical, __VA_ARGS__)

template <>
struct std::formatter<glm::vec2> : std::formatter<std::string>
{
    auto format(const glm::vec2& v, format_context& ctx) { return std::format_to(ctx.out(), "[{}, {}]", v.x, v.y); }
};

template <>
struct std::formatter<glm::vec3> : std::formatter<std::string>
{
    auto format(const glm::vec3& v, format_context& ctx) { return std::format_to(ctx.out(), "[{}, {}, {}]", v.x, v.y, v.z); }
};

template <>
struct std::formatter<glm::vec4> : std::formatter<std::string>
{
    auto format(const glm::vec4& v, format_context& ctx) { return std::format_to(ctx.out(), "[{}, {}, {}, {}]", v.x, v.y, v.z, v.w); }
};

template <>
struct std::formatter<glm::ivec2> : std::formatter<std::string>
{
    auto format(const glm::ivec2& v, format_context& ctx) { return std::format_to(ctx.out(), "[{}, {}]", v.x, v.y); }
};

template <>
struct std::formatter<glm::ivec3> : std::formatter<std::string>
{
    auto format(const glm::ivec3& v, format_context& ctx) { return std::format_to(ctx.out(), "[{}, {}, {}]", v.x, v.y, v.z); }
};

template <>
struct std::formatter<glm::ivec4> : std::formatter<std::string>
{
    auto format(const glm::ivec4& v, format_context& ctx) { return std::format_to(ctx.out(), "[{}, {}, {}, {}]", v.x, v.y, v.z, v.w); }
};