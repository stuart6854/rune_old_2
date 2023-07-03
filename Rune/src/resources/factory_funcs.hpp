#pragma once

#include "internal_common.hpp"
#include "resources/metadata.hpp"
#include "resources/resource.hpp"

#include <memory>

namespace rune::resources
{
    auto factory_load_static_mesh(const Metadata& metadata) -> std::unique_ptr<Resource>;
    auto factory_load_texture(const Metadata& metadata) -> std::unique_ptr<Resource>;
    auto factory_load_material(const Metadata& metadata) -> std::unique_ptr<Resource>;

}