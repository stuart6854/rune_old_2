#pragma once

#include "common.hpp"
#include "scenes/system_scene.hpp"

#include <glm/ext/vector_float3.hpp>

namespace rune::scriptglue
{
#pragma region Entity

    void register_entity_types();

    auto entity_create() -> Entity;
    void entity_destroy(Entity* entity);

    auto entity_position_get(Entity* entity) -> glm::vec3;
    void entity_position_set(Entity* entity, glm::vec3 position);

#pragma endregion
}