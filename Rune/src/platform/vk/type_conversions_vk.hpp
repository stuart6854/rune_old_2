#pragma once

#include "rune/rendering/rhi/common.hpp"
#include "rune/rendering/rhi/command_list.hpp"
#include "rune/rendering/rhi/buffer.hpp"
#include "rune/rendering/rhi/image.hpp"
#include "rune/rendering/rhi/shader_program.hpp"
#include "rune/rendering/rhi/pipeline_state.hpp"

#include <vulkan/vulkan.hpp>

namespace rune::rhi
{
    auto convert(Format format) -> vk::Format;
    auto convert(vk::Format format) -> Format;

    auto convert(BufferType type) -> vk::BufferUsageFlags;

    auto convert(ImageUsage usage) -> vk::ImageUsageFlags;
    auto convert(vk::ImageUsageFlagBits usage) -> ImageUsage;

    auto convert(ShaderType type) -> vk::ShaderStageFlagBits;

    auto convert_to_layout(ResourceState state) -> vk::ImageLayout;
    auto convert_to_stage(ResourceState state) -> vk::PipelineStageFlags2;
    auto convert_to_access(ResourceState state) -> vk::AccessFlags2;

    auto convert(PrimitiveTopology topology) -> vk::PrimitiveTopology;
    auto convert(CullMode cullMode) -> vk::CullModeFlags;

    auto convert(IndexType indexType) -> vk::IndexType;

}