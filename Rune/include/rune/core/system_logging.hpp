#pragma once

#include "common.hpp"
#include "system.hpp"

#include <memory>
#include <filesystem>
#include <string_view>
#include <source_location>

namespace rune
{
    enum class LogLevel : u8
    {
        Trace,
        Debug,
        Info,
        Warn,
        Error,
        Critical,
    };

    class SystemLogging : public ISystem
    {
    public:
        explicit SystemLogging(const std::filesystem::path& logDir);
        ~SystemLogging() override;

        void initialise() override;
        void update() override;
        void shutdown() override;

        void flush();

        void log(const std::source_location& sourceLocation, LogLevel level, std::string_view msg);

        template <typename... Args>
        void log(const std::source_location& sourceLocation, LogLevel level, std::string_view fmt, Args&&... args);

    private:
        struct Pimpl;
        std::unique_ptr<Pimpl> m_pimpl;
    };

    template <typename... Args>
    void SystemLogging::log(const std::source_location& sourceLocation, LogLevel level, std::string_view fmt, Args&&... args)
    {
        const auto msg = std::vformat(fmt, std::make_format_args(args...));
        log(sourceLocation, level, msg);
    }
}
