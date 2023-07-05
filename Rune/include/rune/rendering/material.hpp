#pragma once

#include "renderer_resource.hpp"
#include "texture.hpp"
#include "resources/handle.hpp"

#include <gfx/gfx.hpp>

#include <vector>

namespace rune
{
    class Material : public RendererResource
    {
    public:
        explicit Material(SystemRenderer& renderer);
        ~Material() override;

        void set_textures(const std::vector<ResourceHandle<Texture>>& textures);

        auto get_texture_set() const -> sm::gfx::DescriptorSetHandle { return m_textureSet; }

    private:
        std::vector<ResourceHandle<Texture>> m_textures{};

        sm::gfx::DescriptorSetHandle m_textureSet{};
    };
}