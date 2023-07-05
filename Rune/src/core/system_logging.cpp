#include "core/system_logging.hpp"

#include "common_internal.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace rune
{
    namespace
    {
        auto convert_log_level(LogLevel level) -> spdlog::level::level_enum
        {
            switch (level)
            {
                case LogLevel::Trace: return spdlog::level::trace;
                case LogLevel::Debug: return spdlog::level::debug;
                case LogLevel::Info: return spdlog::level::info;
                case LogLevel::Warn: return spdlog::level::warn;
                case LogLevel::Error: return spdlog::level::err;
                case LogLevel::Critical: return spdlog::level::critical;
            }
            return {};
        }
    }

    struct SystemLogging::Pimpl
    {
        std::filesystem::path logDir{};
        std::unique_ptr<spdlog::logger> defaultLogger{ nullptr };
    };

    SystemLogging::SystemLogging(const std::filesystem::path& logDir) : m_pimpl(new Pimpl{ logDir }) {}

    SystemLogging::~SystemLogging() = default;

    void SystemLogging::initialise()
    {
        auto logFilename = m_pimpl->logDir / "log.txt";

        std::vector<spdlog::sink_ptr> sinks{
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>(),
            std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilename.string(), true),
        };
        m_pimpl->defaultLogger = std::make_unique<spdlog::logger>("default", sinks.begin(), sinks.end());
        m_pimpl->defaultLogger->set_pattern("%T:%e | %^%-8l%$ | thread=%-6t | %s:%# - %v");

#if defined(RUNE_BUILD_DEBUG) || defined(RUNE_BUILD_RELEASE)
        spdlog::set_level(spdlog::level::trace);
#elif defined(RUNE_BUILD_DISTRO)
        spdlog::set_level(spdlog::level::info);
#endif

        LOG_INFO("Logging system initialised.");
    }

    void SystemLogging::update() {}

    void SystemLogging::shutdown()
    {
        LOG_INFO("Logging system is shutting down.");

        flush();
        m_pimpl->defaultLogger = nullptr;
        spdlog::shutdown();
    }

    void SystemLogging::flush()
    {
        m_pimpl->defaultLogger->flush();
    }

    void SystemLogging::log(const std::source_location& sourceLocation, LogLevel level, std::string_view msg)
    {
        auto spdlogSrcLoc = spdlog::source_loc{ sourceLocation.file_name(), i32(sourceLocation.line()), sourceLocation.function_name() };
        m_pimpl->defaultLogger->log(spdlogSrcLoc, convert_log_level(level), msg);
    }

}