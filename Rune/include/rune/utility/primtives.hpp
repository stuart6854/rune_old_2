// #TODO: Generate cubes
// #TODO: Generate uv spheres
// #TODO: Generate ico spheres
// #TODO: Generate capsules
// #TODO: Generate cones

#pragma once

#include "common.hpp"
#include "graphics/renderer/vertices.hpp"

#include <glm/ext/vector_uint2.hpp>

#include <vector>

namespace rune::utility::primitives
{
    struct MeshDescription
    {
        std::vector<graphics::StaticVertex> vertices{};
        std::vector<std::uint32_t> triangles{};
    };

    auto generate_plane(const glm::vec2& size = { 1.0f, 1.0f },
                        const glm::vec2& offset = { 0.0f, 0.0f },
                        const glm::uvec2& resolution = { 1, 1 }) -> MeshDescription;
#if 0
    void generate_cube();
    void generate_uv_sphere();
    void generate_ico_sphere();
    void generate_capsule();
    void generate_cone();
#endif
}
