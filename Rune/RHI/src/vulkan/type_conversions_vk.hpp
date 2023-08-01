#pragma once

#include "rune/rhi/enums.hpp"

#include <vulkan/vulkan.hpp>

namespace rune::rhi
{
    auto convert(Format format) -> vk::Format;

    auto convert(ShaderStages stage) -> vk::ShaderStageFlagBits;

    auto convert(Topology topology) -> vk::PrimitiveTopology;
    auto convert(CullMode cullMode) -> vk::CullModeFlags;
}