/*
 * https://www.danielsieger.com/blog/2021/05/03/generating-primitive-shapes.html
 */

#include "utility/primitives.hpp"

#include "internal_common.hpp"
#include "graphics/renderer/renderer.hpp"

namespace rune::utility::primitives
{
    auto generate_plane(const glm::vec2& size, const glm::vec2& offset, const glm::uvec2& resolution)
        -> std::shared_ptr<graphics::renderer::StaticMesh>
    {
        glm::vec3 position{ offset.x, 0, offset.y };
        glm::vec3 normal{ 0, 1, 0 };
        glm::vec2 texCoord{};

        std::vector<glm::vec3> positions{};
        std::vector<glm::vec3> normals{};
        std::vector<glm::vec2> texCoords{};

        const auto posIncrement = size / glm::vec2(resolution);
        const auto uIncrement = 1.0f / f32(resolution.x);
        const auto vIncrement = 1.0f / f32(resolution.y);
        for (sizet y = 0; y < resolution.y + 1; ++y)
        {
            for (sizet x = 0; x < resolution.x + 1; ++x)
            {
                positions.push_back(position);
                normals.push_back(normal);
                texCoords.push_back(texCoord);

                position.x += posIncrement.x;
                texCoord.x += uIncrement;
            }
            position.x = 0.0f;
            position.z += posIncrement.y;
            texCoord.y += vIncrement;
        }

        std::vector<u32> triangles{};
        for (u32 y = 0; y < resolution.y; ++y)
        {
            for (u32 x = 0; x < resolution.x; ++x)
            {
                u32 v0 = x + y * (resolution.x + 1);
                u32 v1 = v0 + resolution.x + 1;
                u32 v2 = v0 + resolution.x + 2;
                u32 v3 = v0 + 1;
                triangles.insert(triangles.end(), { v0, v1, v2, v2, v3, v0 });
            }
        }

        auto mesh = graphics::renderer::create_static_mesh();
        mesh->set_positions(positions);
        mesh->set_normals(normals);
        mesh->set_tex_coords(texCoords);
        return mesh;
    }

    auto generate_cube(f32 size, const glm::vec3& offset) -> std::shared_ptr<graphics::renderer::StaticMesh>
    {
        const auto hs = size = 0.5f;

        glm::vec3 position{};
        glm::vec3 normal{ 0, 1, 0 };
        glm::vec2 texCoord{};

        std::vector<glm::vec3> positions{
            // Top
            offset + glm::vec3{ 0.0f, 1.0f, 0.0f } * hs,
            offset + glm::vec3{ 0.0f, 1.0f, 0.0f } * hs,
            offset + glm::vec3{ 0.0f, 1.0f, 0.0f } * hs,
            offset + glm::vec3{ 0.0f, 1.0f, 0.0f } * hs,
            // Bottom
            offset + glm::vec3{ 0.0f, -1.0f, 0.0f } * hs,
            offset + glm::vec3{ 0.0f, -1.0f, 0.0f } * hs,
            offset + glm::vec3{ 0.0f, -1.0f, 0.0f } * hs,
            offset + glm::vec3{ 0.0f, -1.0f, 0.0f } * hs,
            // South
            offset + glm::vec3{ 0.0f, 0.0f, -1.0f } * hs,
            offset + glm::vec3{ 0.0f, 0.0f, -1.0f } * hs,
            offset + glm::vec3{ 0.0f, 0.0f, -1.0f } * hs,
            offset + glm::vec3{ 0.0f, 0.0f, -1.0f } * hs,
            // North
            offset + glm::vec3{ 0.0f, 0.0f, 1.0f } * hs,
            offset + glm::vec3{ 0.0f, 0.0f, 1.0f } * hs,
            offset + glm::vec3{ 0.0f, 0.0f, 1.0f } * hs,
            offset + glm::vec3{ 0.0f, 0.0f, 1.0f } * hs,
            // East
            offset + glm::vec3{ 1.0f, 0.0f, 0.0f } * hs,
            offset + glm::vec3{ 1.0f, 0.0f, 0.0f } * hs,
            offset + glm::vec3{ 1.0f, 0.0f, 0.0f } * hs,
            offset + glm::vec3{ 1.0f, 0.0f, 0.0f } * hs,
            // West
            offset + glm::vec3{ -1.0f, 0.0f, 0.0f } * hs,
            offset + glm::vec3{ -1.0f, 0.0f, 0.0f } * hs,
            offset + glm::vec3{ -1.0f, 0.0f, 0.0f } * hs,
            offset + glm::vec3{ -1.0f, 0.0f, 0.0f } * hs,
        };
        std::vector<glm::vec3> normals{
            // Top
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            // Bottom
            { 0.0f, -1.0f, 0.0f },
            { 0.0f, -1.0f, 0.0f },
            { 0.0f, -1.0f, 0.0f },
            { 0.0f, -1.0f, 0.0f },
            // South
            { 0.0f, 0.0f, -1.0f },
            { 0.0f, 0.0f, -1.0f },
            { 0.0f, 0.0f, -1.0f },
            { 0.0f, 0.0f, -1.0f },
            // North
            { 0.0f, 0.0f, 1.0f },
            { 0.0f, 0.0f, 1.0f },
            { 0.0f, 0.0f, 1.0f },
            { 0.0f, 0.0f, 1.0f },
            // East
            { 1.0f, 0.0f, 0.0f },
            { 1.0f, 0.0f, 0.0f },
            { 1.0f, 0.0f, 0.0f },
            { 1.0f, 0.0f, 0.0f },
            // West
            { -1.0f, 0.0f, 0.0f },
            { -1.0f, 0.0f, 0.0f },
            { -1.0f, 0.0f, 0.0f },
            { -1.0f, 0.0f, 0.0f },
        };
        std::vector<glm::vec2> texCoords{
            // Top
            { 0.0f, 0.0f },
            { 0.0f, 0.0f },
            { 0.0f, 0.0f },
            { 0.0f, 0.0f },
            // Bottom
            { 0.0f, 0.0f },
            { 0.0f, 0.0f },
            { 0.0f, 0.0f },
            { 0.0f, 0.0f },
            // South
            { 0.0f, 0.0f },
            { 0.0f, 0.0f },
            { 0.0f, 0.0f },
            { 0.0f, 0.0f },
            // North
            { 0.0f, 0.0f },
            { 0.0f, 0.0f },
            { 0.0f, 0.0f },
            { 0.0f, 0.0f },
            // East
            { 0.0f, 0.0f },
            { 0.0f, 0.0f },
            { 0.0f, 0.0f },
            { 0.0f, 0.0f },
            // West
            { 0.0f, 0.0f },
            { 0.0f, 0.0f },
            { 0.0f, 0.0f },
            { 0.0f, 0.0f },
        };
        // clang-format off
        std::vector<u32> triangles{
            // Top
            0, 1, 2, 2, 3, 0,
            // Bottom
            4, 5, 6, 6, 7, 4,
            // South
            8, 9, 10, 10, 11, 8,
            // North
            12, 13, 14, 14, 15, 12,
            // East
            15, 16, 17, 17, 18, 15,
            // West
            18, 19, 20, 20, 21, 18,
        };
        // clang-format on

        auto mesh = graphics::renderer::create_static_mesh();
        mesh->set_positions(positions);
        mesh->set_normals(normals);
        mesh->set_tex_coords(texCoords);
        mesh->set_triangles(triangles);
        return mesh;
    }

}