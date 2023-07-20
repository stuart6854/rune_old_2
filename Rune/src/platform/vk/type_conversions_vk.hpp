#pragma once

#include "rune/rendering/rhi/common.hpp"
#include "rune/rendering/rhi/buffer.hpp"
#include "rune/rendering/rhi/image.hpp"

#include <vulkan/vulkan.hpp>

namespace rune::rhi
{
    auto convert(Format format) -> vk::Format;
    auto convert(vk::Format format) -> Format;

    auto convert(BufferType type) -> vk::BufferUsageFlags;

    auto convert(ImageUsage usage) -> vk::ImageUsageFlags;
    auto convert(vk::ImageUsageFlagBits usage) -> ImageUsage;

    auto convert_to_layout(ResourceState state) -> vk::ImageLayout;
    auto convert_to_stage(ResourceState state) -> vk::PipelineStageFlags2;
    auto convert_to_access(ResourceState state) -> vk::AccessFlags2;

}