// #TODO: Generate uv spheres
// #TODO: Generate ico spheres
// #TODO: Generate capsules
// #TODO: Generate cones

#pragma once

#include "common.hpp"
#include "graphics/renderer/static_mesh.hpp"

#include <glm/ext/vector_uint2.hpp>

#include <vector>
#include <memory>

namespace rune::utility::primitives
{
    auto generate_plane(const glm::vec2& size = { 1.0f, 1.0f },
                        const glm::vec2& offset = { 0.0f, 0.0f },
                        const glm::uvec2& resolution = { 1, 1 }) -> std::unique_ptr<graphics::renderer::StaticMesh>;
    auto generate_cube(f32 size = 1.0f, const glm::vec3& offset = { 0.0f, 0.0f, 0.0f }) -> std::unique_ptr<graphics::renderer::StaticMesh>;
#if 0
    void generate_uv_sphere();
    void generate_ico_sphere();
    void generate_capsule();
    void generate_cone();
#endif
}
