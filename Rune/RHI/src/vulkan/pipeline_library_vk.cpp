#include "pipeline_library_vk.hpp"

#include "device_vk.hpp"
#include "type_conversions_vk.hpp"

#include <vulkan/vulkan_hash.hpp>

#include <functional>

namespace rune::rhi
{
    PipelineLibraryVulkan::PipelineLibraryVulkan(std::shared_ptr<DeviceInternal>& device) : m_device(device)
    {
        vk::PipelineCacheCreateInfo cacheInfo{};
        m_cache = device->device.createPipelineCache(cacheInfo);
    }

    PipelineLibraryVulkan::~PipelineLibraryVulkan()
    {
        m_device->device.destroy(m_cache);
    }

    auto PipelineLibraryVulkan::get_or_build(const ShaderProgramDesc& desc) -> vk::Pipeline
    {
        const auto hash = std::hash<ShaderProgramDesc>{}(desc);

        auto it = m_pipelineMap.find(hash);
        if (it != m_pipelineMap.end())
            return it->second;

        std::vector<vk::PipelineShaderStageCreateInfo> shaderStages{};
        create_shader_stages(desc, shaderStages);

        std::vector<vk::VertexInputAttributeDescription> vertexAttributes{};
        std::vector<vk::VertexInputBindingDescription> vertexBindings{};
        create_vertex_input(desc.vertexInputFormat, vertexAttributes, vertexBindings);

        vk::PipelineVertexInputStateCreateInfo vertexInputState{};
        vertexInputState.setVertexAttributeDescriptions(vertexAttributes);
        vertexInputState.setVertexBindingDescriptions(vertexBindings);

        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState{};
        inputAssemblyState.setTopology(vk::PrimitiveTopology::eTriangleList);

        vk::PipelineTessellationStateCreateInfo tessellationState{};

        vk::PipelineViewportStateCreateInfo viewportState({}, 1, nullptr, 1, nullptr);

        vk::PipelineRasterizationStateCreateInfo rasterizationState{};
        vk::PipelineMultisampleStateCreateInfo multisampleState{};
        vk::PipelineDepthStencilStateCreateInfo depthStencilState{};

        std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachments{
            vk::PipelineColorBlendAttachmentState(false),
        };
        vk::PipelineColorBlendStateCreateInfo colorBlendState{};
        colorBlendState.setAttachments(colorBlendAttachments);

        std::vector<vk::DynamicState> dynamicStates = {
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor,
            vk::DynamicState::eLineWidth,
            /*vk::DynamicState::eDepthBias,*/
            /*vk::DynamicState::eDepthBiasEnable,*/
            vk::DynamicState::eCullMode,
            vk::DynamicState::eFrontFace,
            vk::DynamicState::ePrimitiveTopology,
            vk::DynamicState::eDepthTestEnable,
            vk::DynamicState::eDepthWriteEnable,
            vk::DynamicState::eStencilTestEnable,
        };
        vk::PipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.setDynamicStates(dynamicStates);

        vk::PipelineLayout layout = m_device->device.createPipelineLayout({});

        std::vector colorFormats = { vk::Format::eB8G8R8A8Srgb };
        vk::PipelineRenderingCreateInfo renderingInfo{};
        // renderingInfo.setColorAttachmentFormats(colorFormats);
        // renderingInfo.setDepthAttachmentFormat({});

        vk::GraphicsPipelineCreateInfo createInfo{};
        createInfo.setStages(shaderStages);
        createInfo.setPVertexInputState(&vertexInputState);
        createInfo.setPInputAssemblyState(&inputAssemblyState);
        createInfo.setPTessellationState(&tessellationState);
        createInfo.setPViewportState(&viewportState);
        createInfo.setPRasterizationState(&rasterizationState);
        createInfo.setPMultisampleState(&multisampleState);
        createInfo.setPDepthStencilState(&depthStencilState);
        createInfo.setPColorBlendState(&colorBlendState);
        createInfo.setPDynamicState(&dynamicState);
        createInfo.setLayout(layout);
        createInfo.setRenderPass(nullptr);
        createInfo.setPNext(&renderingInfo);
        auto pipeline = m_device->device.createGraphicsPipeline(m_cache, createInfo).value;
        m_pipelineMap[hash] = pipeline;
        return pipeline;
    }

    void PipelineLibraryVulkan::create_shader_stages(const ShaderProgramDesc& desc,
                                                     std::vector<vk::PipelineShaderStageCreateInfo>& outStages)
    {
#define CREATE_SHADER_MODULE(_stage, _byteCode)                                      \
    if (!_byteCode.empty())                                                          \
    {                                                                                \
        vk::ShaderModuleCreateInfo moduleInfo{};                                     \
        moduleInfo.pCode = reinterpret_cast<const std::uint32_t*>(_byteCode.data()); \
        moduleInfo.codeSize = _byteCode.size();                                      \
        auto module = m_device->device.createShaderModule(moduleInfo);               \
                                                                                     \
        auto& stage = outStages.emplace_back();                                      \
        stage.setModule(module);                                                     \
        stage.setStage(_stage);                                                      \
        stage.setPName("main");                                                      \
    }

        CREATE_SHADER_MODULE(vk::ShaderStageFlagBits::eCompute, desc.stages.compute.byteCode);
        CREATE_SHADER_MODULE(vk::ShaderStageFlagBits::eVertex, desc.stages.vertex.byteCode);
        CREATE_SHADER_MODULE(vk::ShaderStageFlagBits::eFragment, desc.stages.fragment.byteCode);
    }

    void PipelineLibraryVulkan::create_vertex_input(const VertexInputFormat& vertexInputFormat,
                                                    std::vector<vk::VertexInputAttributeDescription>& outAttributes,
                                                    std::vector<vk::VertexInputBindingDescription>& outBindings)
    {
        for (const auto& attribute : vertexInputFormat.Attributes)
        {
            auto& outAttribute = outAttributes.emplace_back();
            outAttribute.setBinding(attribute.binding);
            outAttribute.setLocation(attribute.location);
            outAttribute.setFormat(convert(attribute.format));
            outAttribute.setOffset(attribute.offset);
        }

        for (const auto& binding : vertexInputFormat.Bindings)
        {
            auto& outBinding = outBindings.emplace_back();
            outBinding.setBinding(binding.binding);
            outBinding.setStride(binding.stride);
        }
    }

}