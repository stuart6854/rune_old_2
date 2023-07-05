#include <rune/entry_point.hpp>

#include <iostream>

void rune::configure_engine(rune::EngineConfig& config)
{
    config.logDirPath = "../../logs";
    config.configFilename = "../../config.ini";
    config.initialScene = "scene_01.yaml";
}
