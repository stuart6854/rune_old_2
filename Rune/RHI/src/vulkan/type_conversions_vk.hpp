#pragma once

#include "rune/rhi/enums.hpp"

#include <vulkan/vulkan.hpp>
#include <vulkan-memory-allocator-hpp/vk_mem_alloc.hpp>

namespace rune::rhi
{
    auto convert(Format format) -> vk::Format;

    auto convert(ShaderStages stage) -> vk::ShaderStageFlagBits;

    auto convert(Topology topology) -> vk::PrimitiveTopology;
    auto convert(CullMode cullMode) -> vk::CullModeFlags;

    auto convert_buffer(UsageFlags usage) -> vk::BufferUsageFlagBits;
    auto convert(ReadWriteUsage usage) -> vma::MemoryUsage;
}