#pragma once

#include "rune/common.hpp"
#include "rune/debug/log.hpp"

#include <format>
#include <string_view>

namespace rune::debug
{
    void log_engine(const char* file, i32 line, LogLevel level, std::string_view msg);

    template <typename... Args>
    void log_engine(const char* file, i32 line, LogLevel level, std::string_view fmt, Args&&... args)
    {
        log_engine(file, line, level, std::format(fmt, std::forward(args)...));
    }
}

#define RUNE_ENG_TRACE(...) ::rune::debug::log_engine(__FILE__, __LINE__, ::rune::debug::LogLevel::Trace, __VA_ARGS__)
#define RUNE_ENG_DEBUG(...) ::rune::debug::log_engine(__FILE__, __LINE__, ::rune::debug::LogLevel::Debug, __VA_ARGS__)
#define RUNE_ENG_INFO(...) ::rune::debug::log_engine(__FILE__, __LINE__, ::rune::debug::LogLevel::Info, __VA_ARGS__)
#define RUNE_ENG_WARN(...) ::rune::debug::log_engine(__FILE__, __LINE__, ::rune::debug::LogLevel::Warn, __VA_ARGS__)
#define RUNE_ENG_ERROR(...) ::rune::debug::log_engine(__FILE__, __LINE__, ::rune::debug::LogLevel::Error, __VA_ARGS__)
#define RUNE_ENG_CRITICAL(...) ::rune::debug::log_engine(__FILE__, __LINE__, ::rune::debug::LogLevel::Critical, __VA_ARGS__)