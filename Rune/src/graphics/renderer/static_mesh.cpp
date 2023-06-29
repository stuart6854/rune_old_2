#include "graphics/renderer/static_mesh.hpp"

namespace rune::graphics::renderer
{
    void StaticMesh::set_positions(const std::vector<glm::vec3>& positions)
    {
        m_positions = positions;
    }

    void StaticMesh::set_normals(const std::vector<glm::vec3>& normals)
    {
        m_normals = normals;
    }

    void StaticMesh::set_tex_coords(const std::vector<glm::vec2>& texCoords)
    {
        m_texCoords = texCoords;
    }

    void StaticMesh::apply()
    {
        void* mappedData{ nullptr };

        gfx::destroy_buffer(m_positionsBuffer);
        gfx::destroy_buffer(m_otherAttribBuffer);

        gfx::BufferInfo bufferInfo{
            .type = gfx::BufferType::eVertex,
            .size = sizeof(glm::vec3) * m_positions.size(),
        };
        gfx::create_buffer(m_positionsBuffer, graphics::get_device(), bufferInfo);
        gfx::map_buffer(m_positionsBuffer, mappedData);
        std::memcpy(mappedData, m_positions.data(), bufferInfo.size);
        gfx::unmap_buffer(m_positionsBuffer);
        m_positions.clear();  // #TODO: Allow user to specify to keep vertex data

        bufferInfo.size = (sizeof(glm::vec3) * m_normals.size()) + (sizeof(glm::vec2) * m_texCoords.size());
        gfx::create_buffer(m_otherAttribBuffer, graphics::get_device(), bufferInfo);
        gfx::map_buffer(m_otherAttribBuffer, mappedData);
        u8* writePtr = static_cast<u8*>(mappedData);
        for (auto i = 0; i < m_normals.size(); ++i)
        {
            auto offset = i * (sizeof(glm::vec3) + sizeof(glm::vec2));
            std::memcpy(writePtr + offset + 0, &m_normals[i], sizeof(glm::vec3));
            std::memcpy(writePtr + offset + sizeof(glm::vec3), &m_texCoords[i], sizeof(glm::vec2));
        }
        gfx::unmap_buffer(m_otherAttribBuffer);
        m_normals.clear();    // #TODO: Allow user to specify to keep vertex data
        m_texCoords.clear();  // #TODO: Allow user to specify to keep vertex data
    }

}