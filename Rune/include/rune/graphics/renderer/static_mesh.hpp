#pragma once

#include "common.hpp"

#include "graphics/graphics.hpp"

#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>

#include <vector>

namespace rune::graphics::renderer
{
    class StaticMesh
    {
    public:
        StaticMesh() = default;
        ~StaticMesh() = default;

        void set_positions(const std::vector<glm::vec3>& positions);
        void set_normals(const std::vector<glm::vec3>& normals);
        void set_tex_coords(const std::vector<glm::vec2>& texCoords);
        void set_triangles(const std::vector<u32>& triangles);

        void apply();

        auto get_index_buffer() const -> gfx::BufferHandle;
        auto get_vertex_buffers() const -> std::vector<gfx::BufferHandle>;

    private:
        std::vector<glm::vec3> m_positions{};
        std::vector<glm::vec3> m_normals{};
        std::vector<glm::vec2> m_texCoords{};
        std::vector<u32> m_triangles{};

        gfx::BufferHandle m_positionsBuffer{};
        gfx::BufferHandle m_otherAttribBuffer{};
        gfx::BufferHandle m_indexBuffer{};
    };
}
