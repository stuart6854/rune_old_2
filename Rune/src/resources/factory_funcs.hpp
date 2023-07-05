#pragma once

#include "common_internal.hpp"
#include "resources/system_resources.hpp"
#include "resources/metadata.hpp"
#include "resources/resource.hpp"

#include <memory>

namespace rune
{
    auto factory_load_static_mesh(SystemResources& resourceSystem, const ResourceMetadata& metadata) -> std::unique_ptr<Resource>;
    auto factory_load_texture(SystemResources& resourceSystem, const ResourceMetadata& metadata) -> std::unique_ptr<Resource>;
    auto factory_load_material(SystemResources& resourceSystem, const ResourceMetadata& metadata) -> std::unique_ptr<Resource>;

}