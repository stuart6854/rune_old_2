#pragma once

#include "common.hpp"

#include <glm/ext/vector_float3.hpp>

namespace rune::scenes
{
#define DEFINE_COMPONENT_CTORS(_ComponentType) \
    _ComponentType() = default;                \
    _ComponentType(const _ComponentType&) = default

    struct Transform
    {
        DEFINE_COMPONENT_CTORS(Transform);

        glm::vec3 position{};
        glm::vec3 eulerAngles{};
        glm::vec3 scale{ 1, 1, 1 };
    };
}