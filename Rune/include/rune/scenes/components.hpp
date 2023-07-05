#pragma once

#include "common.hpp"
#include "platform/system_platform.hpp"
#include "resources/handle.hpp"
#include "rendering/static_mesh.hpp"
#include "rendering/material.hpp"

#include <glm/ext/vector_float3.hpp>
#include <glm/ext/quaternion_float.hpp>

#include <memory>

#define DEFINE_COMPONENT_CTORS(_ComponentType) \
    _ComponentType() = default;                \
    _ComponentType(const _ComponentType&) = default

namespace rune
{
    struct Transform
    {
        DEFINE_COMPONENT_CTORS(Transform);

        glm::vec3 position{};
        glm::quat rotation{};
        //        glm::vec3 eulerAngles{};
        glm::vec3 scale{ 1, 1, 1 };

        auto get_forward() const -> glm::vec3 { return rotation * glm::vec3{ 0, 0, 1 }; };
        auto get_up() const -> glm::vec3 { return rotation * glm::vec3{ 0, 1, 0 }; };
        auto get_right() const -> glm::vec3 { return rotation * glm::vec3{ 1, 0, 0 }; };
    };

    struct StaticRenderer
    {
        DEFINE_COMPONENT_CTORS(StaticRenderer);

        ResourceHandle<StaticMesh> mesh{ nullptr };
        std::vector<ResourceHandle<Material>> materials{};
    };

    struct Camera
    {
        DEFINE_COMPONENT_CTORS(Camera);

        WindowHandle targetWindow{};
    };

}