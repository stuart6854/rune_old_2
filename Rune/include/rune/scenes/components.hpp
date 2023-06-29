#pragma once

#include "common.hpp"
#include "graphics/renderer/static_mesh.hpp"

#include <glm/ext/vector_float3.hpp>

#include <memory>

#define DEFINE_COMPONENT_CTORS(_ComponentType) \
    _ComponentType() = default;                \
    _ComponentType(const _ComponentType&) = default

namespace rune::scenes
{
    struct Transform
    {
        DEFINE_COMPONENT_CTORS(Transform);

        glm::vec3 position{};
        glm::vec3 eulerAngles{};
        glm::vec3 scale{ 1, 1, 1 };
    };

    struct StaticRenderer
    {
        DEFINE_COMPONENT_CTORS(StaticRenderer);

        std::shared_ptr<graphics::renderer::StaticMesh> mesh{ nullptr };
    };
}