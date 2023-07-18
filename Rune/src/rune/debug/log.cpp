#include "rune/debug/log.hpp"
#include "log_engine.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace rune::debug
{
    static Shared<spdlog::logger> s_engineLogger{ nullptr };
    static Shared<spdlog::logger> s_clientLogger{ nullptr };

    void initialise(const std::string& logOutputDir)
    {
        spdlog::set_level(spdlog::level::trace);

        auto sharedFileSink = create_shared<spdlog::sinks::basic_file_sink_mt>(logOutputDir + "/log.txt");
        auto sharedConsoleSink = create_shared<spdlog::sinks::stdout_color_sink_mt>();

        std::vector<spdlog::sink_ptr> sinks{
            sharedConsoleSink,
            sharedFileSink,
        };
        s_engineLogger = create_shared<spdlog::logger>("Engine", sinks.begin(), sinks.end());
        s_clientLogger = create_shared<spdlog::logger>("Client", sinks.begin(), sinks.end());
    }

    void shutdown() {}

    void log_engine(const char* file, i32 line, LogLevel level, std::string_view msg)
    {
        s_engineLogger->log(spdlog::source_loc{ file, line, "" }, spdlog::level::level_enum(level), msg);
    }

    void log_client(const char* file, i32 line, LogLevel level, std::string_view msg)
    {
        s_clientLogger->log(spdlog::source_loc{ file, line, "" }, spdlog::level::level_enum(level), msg);
    }

}