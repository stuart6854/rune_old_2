#pragma once

#include "common.hpp"

#include "renderer_resource.hpp"

#include <gfx/gfx.hpp>

#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>

#include <vector>

namespace rune
{
    class StaticMesh : public RendererResource
    {
    public:
        explicit StaticMesh(SystemRenderer& renderer) : RendererResource(renderer) {}
        ~StaticMesh() override = default;

        void set_positions(const std::vector<glm::vec3>& positions);
        void set_normals(const std::vector<glm::vec3>& normals);
        void set_tex_coords(const std::vector<glm::vec2>& texCoords);
        void set_triangles(const std::vector<u32>& triangles);

        void apply();

        auto get_index_count() const -> u32;
        auto get_index_buffer() const -> sm::gfx::BufferHandle;
        auto get_vertex_buffers() const -> std::vector<sm::gfx::BufferHandle>;

    private:
        std::vector<glm::vec3> m_positions{};
        std::vector<glm::vec3> m_normals{};
        std::vector<glm::vec2> m_texCoords{};
        std::vector<u32> m_triangles{};

        sm::gfx::BufferHandle m_positionsBuffer{};
        sm::gfx::BufferHandle m_otherAttribBuffer{};
        sm::gfx::BufferHandle m_indexBuffer{};
        u32 m_indexCount;
    };
}
