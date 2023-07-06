#pragma once

#include "common.hpp"
#include "scenes/system_scene.hpp"

#include <glm/ext/vector_float3.hpp>

namespace rune::scriptglue
{
    void register_functions();

#pragma region Time

    void register_time_functions();

    auto Time_GetDelta() -> float;
    auto Time_GetSinceStartup() -> float;

#pragma endregion

#pragma region Entity

    void register_entity_functions();

    auto Entity_Create() -> Entity;
    void Entity_Destroy(Entity* entity);

    void Entity_GetPosition(Entity* entity, glm::vec3* position);
    void Entity_SetPosition(Entity* entity, glm::vec3* position);

#pragma endregion
}