#include <rune/entry_point.hpp>

#include <iostream>

void rune::configure_engine(rune::EngineConfig& config)
{
    std::cout << "Engine config\n";

    config.logDirPath = "./logs";
    config.configFilename = "./config.ini";
}
