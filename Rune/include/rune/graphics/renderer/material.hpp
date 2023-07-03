#pragma once

#include "texture.hpp"
#include "resources/resource.hpp"
#include "resources/handle.hpp"

#include <vector>

namespace rune::graphics::renderer
{
    struct Material : public resources::Resource
    {
        std::vector<resources::ResourceHandle<Texture>> textures{};
    };
}