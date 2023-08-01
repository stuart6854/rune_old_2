#pragma once

#include "rune/rhi/structs.hpp"

#include <vulkan/vulkan.hpp>

#include <cstddef>
#include <unordered_map>

namespace rune::rhi
{
    struct DeviceInternal;

    class PipelineLibraryVulkan
    {
    public:
        PipelineLibraryVulkan(std::shared_ptr<DeviceInternal>& device);
        ~PipelineLibraryVulkan();

        auto get_or_build(const ShaderProgramDesc& desc) -> vk::Pipeline;

    private:
        void create_shader_stages(const ShaderProgramDesc& desc, std::vector<vk::PipelineShaderStageCreateInfo>& outModules);
        void create_vertex_input(const VertexInputFormat& vertexInputFormat,
                                 std::vector<vk::VertexInputAttributeDescription>& outAttributes,
                                 std::vector<vk::VertexInputBindingDescription>& outBindings);

    private:
        std::shared_ptr<DeviceInternal> m_device = nullptr;
        vk::PipelineCache m_cache = nullptr;
        std::unordered_map<std::size_t, vk::Pipeline> m_pipelineMap = {};
    };
}