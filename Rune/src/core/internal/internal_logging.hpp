#pragma once

#include "internal_common.hpp"

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
    #define LOG_TRACE(...)                                                \
        ::rune::logging::internal::get_logging_data().defaultLogger->log( \
            spdlog::source_loc{ __FILE__, __LINE__, SPDLOG_FUNCTION }, spdlog::level::trace, __VA_ARGS__)
    #define LOG_DEBUG(...)                                                \
        ::rune::logging::internal::get_logging_data().defaultLogger->log( \
            spdlog::source_loc{ __FILE__, __LINE__, SPDLOG_FUNCTION }, spdlog::level::debug, __VA_ARGS__)
    #define LOG_INFO(...)                                                 \
        ::rune::logging::internal::get_logging_data().defaultLogger->log( \
            spdlog::source_loc{ __FILE__, __LINE__, SPDLOG_FUNCTION }, spdlog::level::info, __VA_ARGS__)
    #define LOG_WARN(...)                                                 \
        ::rune::logging::internal::get_logging_data().defaultLogger->log( \
            spdlog::source_loc{ __FILE__, __LINE__, SPDLOG_FUNCTION }, spdlog::level::warn, __VA_ARGS__)
    #define LOG_ERROR(...)                                                \
        ::rune::logging::internal::get_logging_data().defaultLogger->log( \
            spdlog::source_loc{ __FILE__, __LINE__, SPDLOG_FUNCTION }, spdlog::level::err, __VA_ARGS__)
    #define LOG_CRITICAL(...)                                             \
        ::rune::logging::internal::get_logging_data().defaultLogger->log( \
            spdlog::source_loc{ __FILE__, __LINE__, SPDLOG_FUNCTION }, spdlog::level::critical, __VA_ARGS__)
#else
    #define LOG_TRACE(...)
    #define LOG_DEBUG(...)
    #define LOG_INFO(...)
    #define LOG_WARN(...)
    #define LOG_ERROR(...)
    #define LOG_CRITICAL(...)
#endif
