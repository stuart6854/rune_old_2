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

        {
            // Create staging buffer
            gfx::BufferInfo stagingBufferInfo{
                .type = gfx::BufferType::eUpload,
                .size = sizeof(std::uint8_t) * data.size(),
            };
            gfx::BufferHandle stagingBufferHandle{};
            if (!gfx::create_buffer(stagingBufferHandle, get_device(), stagingBufferInfo))
            {
                throw std::runtime_error("Failed to create GFX staging buffer!");
            }

            void* stagingBufferPtr{ nullptr };
            if (gfx::map_buffer(stagingBufferHandle, stagingBufferPtr))
            {
                std::memcpy(stagingBufferPtr, data.data(), stagingBufferInfo.size);
                gfx::unmap_buffer(stagingBufferHandle);
            }

            gfx::CommandListHandle uploadCommandListHandle{};
            if (!gfx::create_command_list(uploadCommandListHandle, get_device(), 0))
            {
                throw std::runtime_error("Failed to create GFX upload command list!");
            }

            gfx::begin(uploadCommandListHandle);

            gfx::transition_texture(uploadCommandListHandle, m_texture, gfx::TextureState::eUndefined, gfx::TextureState::eUploadDst);
            gfx::copy_buffer_to_texture(uploadCommandListHandle, stagingBufferHandle, m_texture);
            gfx::transition_texture(uploadCommandListHandle, m_texture, gfx::TextureState::eUploadDst, gfx::TextureState::eShaderRead);

            gfx::end(uploadCommandListHandle);

            gfx::SubmitInfo submitInfo{ .commandList = uploadCommandListHandle, .waitSemaphoreHandle = {} };
            gfx::FenceHandle fenceHandle{};
            gfx::submit_command_list(submitInfo, &fenceHandle, nullptr);

            gfx::wait_on_fence(fenceHandle);
            gfx::destroy_command_list(get_device(), uploadCommandListHandle);
            gfx::destroy_buffer(stagingBufferHandle);
        }
    }

}