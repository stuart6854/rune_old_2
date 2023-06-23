#pragma once

#include "common.hpp"

#include <spdlog/spdlog.h>

namespace rune::logging::internal
{
    struct LoggingData
    {
        std::shared_ptr<spdlog::logger> defaultLogger;
    };

    auto get_logging_data() -> LoggingData&;

    void initialise(std::string_view logDirName);
    void shutdown();

    void flush();

}

#if defined(RUNE_BUILD_DEBUG) || defined(RUNE_BUILD_RELEASE)
    #define LOG_TRACE(_msg, ...) \
        ::rune::logging::internal::get_logging_data().defaultLogger->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::trace, _msg, __VA_ARGS__)
    #define LOG_DEBUG(_msg, ...) \
        ::rune::logging::internal::get_logging_data().defaultLogger->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::debug, _msg, __VA_ARGS__)
    #define LOG_INFO(_msg, ...) ::rune::logging::internal::get_logging_data().defaultLogger->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::info, _msg, __VA_ARGS__)
    #define LOG_WARN(_msg, ...) ::rune::logging::internal::get_logging_data().defaultLogger->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::warn, _msg, __VA_ARGS__)
    #define LOG_ERROR(_msg, ...) ::rune::logging::internal::get_logging_data().defaultLogger->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::err, _msg, __VA_ARGS__)
    #define LOG_CRITICAL(_msg, ...) \
        ::rune::logging::internal::get_logging_data().defaultLogger->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::critical, _msg, __VA_ARGS__)
#else
    #define LOG_TRACE(_msg, ...)
    #define LOG_DEBUG(_msg, ...)
    #define LOG_INFO(_msg, ...)
    #define LOG_WARN(_msg, ...)
    #define LOG_ERROR(_msg, ...)
    #define LOG_CRITICAL(_msg, ...)
#endif
