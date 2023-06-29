#pragma once

#include "common.hpp"

#include <vector>
#include <string_view>

namespace rune::scenes
{
    enum class LoadMethod
    {
        Normal,
        Additive,
    };

    void initialise();
    void shutdown();

    void update();

    void new_scene();
    void load_scene(std::string_view filename, LoadMethod loadMethod = LoadMethod::Normal);
    void unload_scene();

    /**
     * Creates a new scene and adds some test entities.
     */
    void new_test_scene();

}