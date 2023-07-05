#pragma once

#include "common.hpp"
#include "renderer_resource.hpp"

#include <gfx/gfx.hpp>

namespace rune
{
    class Texture : public RendererResource
    {
    public:
        explicit Texture(SystemRenderer& renderer) : RendererResource(renderer) {}
        ~Texture() override = default;

        void set_dimensions(u32 width, u32 height, u32 depth);

        void set_data(const std::vector<u8>& data);

        auto get_handle() const -> sm::gfx::TextureHandle { return m_texture; }

    private:
        u32 m_width{};
        u32 m_height{};
        u32 m_depth{};
        std::vector<u8> m_data{};

        sm::gfx::TextureHandle m_texture{};
    };
}