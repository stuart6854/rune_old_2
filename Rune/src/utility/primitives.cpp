/*
 * https://www.danielsieger.com/blog/2021/05/03/generating-primitive-shapes.html
 */

#include "utility/primtives.hpp"

#include "internal_common.hpp"

namespace rune::utility::primitives
{
    auto generate_plane(const glm::vec2& size, const glm::vec2& offset, const glm::uvec2& resolution) -> MeshDescription
    {
        MeshDescription meshDesc{};
        graphics::StaticVertex vertex{ .position = { offset.x, 0, offset.y }, .normal = { 0.0f, 1.0f, 0.0f } };

        const auto posIncrement = size / glm::vec2(resolution);
        const auto uIncrement = 1.0f / f32(resolution.x);
        const auto vIncrement = 1.0f / f32(resolution.y);
        for (sizet y = 0; y < resolution.y + 1; ++y)
        {
            for (sizet x = 0; x < resolution.x + 1; ++x)
            {
                meshDesc.vertices.push_back(vertex);

                vertex.position.x += posIncrement.x;
                vertex.texCoord.x += uIncrement;
            }
            vertex.position.x = 0.0f;
            vertex.position.z += posIncrement.y;
            vertex.texCoord.y += vIncrement;
        }

        for (u32 y = 0; y < resolution.y; ++y)
        {
            for (u32 x = 0; x < resolution.x; ++x)
            {
                u32 v0 = x + y * (resolution.x + 1);
                u32 v1 = v0 + resolution.x + 1;
                u32 v2 = v0 + resolution.x + 2;
                u32 v3 = v0 + 1;
                meshDesc.triangles.insert(meshDesc.triangles.end(), { v0, v1, v2, v2, v3, v0 });
            }
        }

        return meshDesc;
    }

}