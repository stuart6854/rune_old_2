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
        explicit Material(gfx::DeviceHandle device) : GraphicsResource(device) {}
        ~Material() override = default;

        std::vector<resources::ResourceHandle<Texture>> textures{};
    };
}