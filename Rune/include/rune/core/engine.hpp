#pragma once

#include <string>

namespace rune
{
    struct EngineConfig
    {
        std::string logOutputDir;
    };

    class Engine
    {
    public:
        bool run(const EngineConfig& config);

    private:
        bool initialise();
        bool shutdown();

        void main_loop();

    private:
        EngineConfig m_config{};
    };
}