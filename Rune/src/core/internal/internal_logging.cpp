#include "internal_logging.hpp"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>

namespace rune::logging::internal
{
    namespace
    {
        std::unique_ptr<LoggingData> s_loggingData{ nullptr };  // NOLINT
    }

    auto get_logging_data() -> LoggingData&
    {
        assert(s_loggingData != nullptr);
        return *s_loggingData;
    }

    void initialise(std::string_view logDirName)
    {
        RUNE_ASSERT(s_loggingData == nullptr);
        s_loggingData = std::make_unique<LoggingData>();

        auto& loggingData = get_logging_data();

        auto logFilename = std::string(logDirName) + "/log.txt";

        std::vector<spdlog::sink_ptr> sinks{
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>(),
            std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilename, true),
        };
        loggingData.defaultLogger = std::make_shared<spdlog::logger>("default", sinks.begin(), sinks.end());
        loggingData.defaultLogger->set_pattern("%T:%e | %^%-8l%$ | thread=%-6t | %s:%# - %v");

        // LOG_TRACE("Trace");
        // LOG_DEBUG("Debug");
        // LOG_INFO("Info");
        // LOG_WARN("Warn");
        // LOG_ERROR("Error");
        // LOG_CRITICAL("Critical");
    }

    void shutdown()
    {
        flush();
        spdlog::shutdown();

        s_loggingData = nullptr;
    }

    void flush()
    {
        auto& loggingData = get_logging_data();
        loggingData.defaultLogger->flush();
    }

}