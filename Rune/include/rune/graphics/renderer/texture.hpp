#pragma once

#include "common.hpp"

#include "graphics/graphics.hpp"
#include "graphics/graphics_resource.hpp"

namespace rune::graphics::renderer
{
    class Texture : public GraphicsResource
    {
    public:
        explicit Texture(gfx::DeviceHandle device) : GraphicsResource(device) {}
        ~Texture() override = default;

        void set_dimensions(u32 width, u32 height, u32 depth);

        void set_data(const std::vector<u8>& data);

    private:
        u32 m_width{};
        u32 m_height{};
        u32 m_depth{};
        std::vector<u8> m_data{};

        gfx::TextureHandle m_texture{};
    };
}