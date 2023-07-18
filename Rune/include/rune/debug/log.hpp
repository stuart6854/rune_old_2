#pragma once

#include "rune/common.hpp"

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
        Err,
        Critical,
        Off,
    };

    void initialise(const std::string& logOutputDir);
    void shutdown();

    void log_client(const char* file, i32 line, LogLevel level, std::string_view msg);

    template <typename... Args>
    void log_client(const char* file, i32 line, LogLevel level, std::string_view fmt, Args&&... args)
    {
        log_client(file, line, level, std::format(fmt, std::forward(args)...));
    }
}

#define RUNE_CLIENT_TRACE(...) ::rune::debug::log_engine(__FILE__, __LINE__, ::rune::debug::LogLevel::Trace, __VA_ARGS__)
#define RUNE_CLIENT_DEBUG(...) ::rune::debug::log_engine(__FILE__, __LINE__, ::rune::debug::LogLevel::Debug, __VA_ARGS__)
#define RUNE_CLIENT_INFO(...) ::rune::debug::log_engine(__FILE__, __LINE__, ::rune::debug::LogLevel::Info, __VA_ARGS__)
#define RUNE_CLIENT_WARN(...) ::rune::debug::log_engine(__FILE__, __LINE__, ::rune::debug::LogLevel::Warn, __VA_ARGS__)
#define RUNE_CLIENT_ERROR(...) ::rune::debug::log_engine(__FILE__, __LINE__, ::rune::debug::LogLevel::Error, __VA_ARGS__)
#define RUNE_CLIENT_CRITICAL(...) ::rune::debug::log_engine(__FILE__, __LINE__, ::rune::debug::LogLevel::Critical, __VA_ARGS__)