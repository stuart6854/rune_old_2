#include "graphics/renderer/texture.hpp"

#include "internal_common.hpp"

namespace rune::graphics::renderer
{
    void Texture::set_dimensions(u32 width, u32 height, u32 depth)
    {
        m_width = width;
        m_height = height;
        m_depth = depth;
    }

    void Texture::set_data(const std::vector<u8>& data)
    {
        RUNE_ASSERT(data.size() == m_width * m_height * m_depth * 4);
        m_data = data;

        gfx::TextureInfo textureInfo{
            .usage = gfx::TextureUsage::eTexture,
            .type = gfx::TextureType::e2D,
            .width = m_width,
            .height = m_height,
            .format = gfx::Format::eRGBA8,
            .mipLevels = 1,
        };
        gfx::create_texture(m_texture, graphics::get_device(), textureInfo);
    }

}