#pragma once

#include "common.hpp"
#include "scenes/system_scene.hpp"

#include <angelscript.h>

namespace rune::scriptglue
{
#pragma region Entity

    void register_entity_types(asIScriptEngine* engine);

    auto create_entity() -> Entity;
    void destroy_entity(Entity* entity);

#pragma endregion
}