#pragma once

#include "common.hpp"
#include "scenes/system_scene.hpp"
#include "mono/metadata/object.h"

#include <mono/metadata/image.h>

#include <glm/ext/vector_float3.hpp>

namespace rune::scriptglue
{
    void register_functions();
    void register_components(MonoImage* image);

#pragma region Time

    auto Time_GetDelta() -> float;
    auto Time_GetSinceStartup() -> float;

#pragma endregion

#pragma region Entity

    auto Entity_Create() -> Entity;
    void Entity_Destroy(Entity* entity);

    bool Entity_HasComponent(const Entity* entity, MonoReflectionType* componentType);

#pragma endregion

#pragma region Transform

    void Transform_GetPosition(const Entity* entity, glm::vec3* position);
    void Transform_SetPosition(const Entity* entity, glm::vec3* position);

#pragma endregion
}