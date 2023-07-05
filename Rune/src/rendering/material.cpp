#include "rendering/material.hpp"

#include "common_internal.hpp"

using namespace sm;

namespace rune
{
    Material::Material(SystemRenderer& renderer) : RendererResource(renderer)
    {
        gfx::DescriptorSetInfo setInfo
        {
            .bindings = {
                { gfx::DescriptorType::eTexture, 1, gfx::ShaderStageFlags_Fragment },
            },
        };
        gfx::create_descriptor_set(m_textureSet, get_renderer().get_device(), setInfo);
    }

    Material::~Material()
    {
        // #TODO: Destroy texture set
    }

    void Material::set_textures(const std::vector<ResourceHandle<Texture>>& textures)
    {
        m_textures = textures;

        gfx::SamplerInfo samplerInfo{
            .addressMode = gfx::SamplerAddressMode::eRepeat,
            .filterMode = gfx::SamplerFilterMode::eLinear,
        };
        gfx::SamplerHandle samplerHandle{};
        gfx::create_sampler(samplerHandle, get_renderer().get_device(), samplerInfo);

        auto* textureToBind = m_textures[0].get();
        gfx::bind_texture_to_descriptor_set(m_textureSet, 0, textureToBind->get_handle(), samplerHandle);
    }

}