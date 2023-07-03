#pragma once

#include "texture.hpp"
#include "graphics/graphics_resource.hpp"
#include "resources/handle.hpp"

#include <vector>

namespace rune::graphics::renderer
{
    class Material : public GraphicsResource
    {
    public:
        explicit Material(gfx::DeviceHandle device);
        ~Material() override;

        void set_textures(const std::vector<resources::ResourceHandle<Texture>>& textures);

        auto get_texture_set() const -> gfx::DescriptorSetHandle { return m_textureSet; }

    private:
        std::vector<resources::ResourceHandle<Texture>> m_textures{};

        gfx::DescriptorSetHandle m_textureSet{};
    };
}