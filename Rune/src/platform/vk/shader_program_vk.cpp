#include "shader_program_vk.hpp"

#include "device_vk.hpp"
#include "type_conversions_vk.hpp"

namespace rune::rhi
{
    auto ShaderProgram::create(Shared<Device> device, const ShaderProgramDecl& decl) -> Owned<ShaderProgram>
    {
        return create_owned<ShaderProgramVulkan>(static_pointer_cast<DeviceVulkan>(device), decl);
    }

    ShaderProgramVulkan::ShaderProgramVulkan(Shared<DeviceVulkan> device, const ShaderProgramDecl& decl) : m_device(device)
    {
        auto vkDevice = m_device->get_vk_device();

        create_shaders(decl.ShaderStages);

        vk::PipelineLayoutCreateInfo layoutInfo{};
        m_layout = vkDevice.createPipelineLayout(layoutInfo);

        vk::PipelineVertexInputStateCreateInfo vertexInputStateInfo{};

        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateInfo{};
        inputAssemblyStateInfo.setTopology(vk::PrimitiveTopology::eTriangleList);

        vk::PipelineTessellationStateCreateInfo tesselationStateInfo{};

        vk::PipelineViewportStateCreateInfo viewportStateInfo{};
        viewportStateInfo.setViewportCount(1);
        viewportStateInfo.setScissorCount(1);

        vk::PipelineRasterizationStateCreateInfo rasterizationStateInfo{};

        vk::PipelineMultisampleStateCreateInfo multisampleStateInfo{};

        vk::PipelineDepthStencilStateCreateInfo depthStencilStateInfo{};

        vk::PipelineColorBlendStateCreateInfo colorBlendStateInfo{};

        // https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html#VkDynamicState
        std::vector<vk::DynamicState> dynamicStates{
            vk::DynamicState::eViewport,          vk::DynamicState::eScissor,          vk::DynamicState::ePrimitiveTopology,
            vk::DynamicState::eCullMode,          vk::DynamicState::eFrontFace,        vk::DynamicState::eLineWidth,
            vk::DynamicState::eDepthTestEnable,   vk::DynamicState::eDepthWriteEnable, vk::DynamicState::eDepthCompareOp,
            vk::DynamicState::eStencilTestEnable, vk::DynamicState::eStencilOp,        vk::DynamicState::eDepthBiasEnable,
            vk::DynamicState::eDepthBias,
        };
        vk::PipelineDynamicStateCreateInfo dynamicStateInfo{};
        dynamicStateInfo.setDynamicStates(dynamicStates);

        vk::PipelineRenderingCreateInfo renderingInfo{};

        vk::GraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.setLayout(m_layout);
        pipelineInfo.setStages(m_stageInfos);
        pipelineInfo.setPVertexInputState(&vertexInputStateInfo);
        pipelineInfo.setPInputAssemblyState(&inputAssemblyStateInfo);
        pipelineInfo.setPTessellationState(&tesselationStateInfo);
        pipelineInfo.setPViewportState(&viewportStateInfo);
        pipelineInfo.setPRasterizationState(&rasterizationStateInfo);
        pipelineInfo.setPMultisampleState(&multisampleStateInfo);
        pipelineInfo.setPDepthStencilState(&depthStencilStateInfo);
        pipelineInfo.setPColorBlendState(&colorBlendStateInfo);
        pipelineInfo.setPDynamicState(&dynamicStateInfo);
        pipelineInfo.setPNext(&renderingInfo);
        m_pipeline = vkDevice.createGraphicsPipeline({}, pipelineInfo).value;
    }

    ShaderProgramVulkan::~ShaderProgramVulkan()
    {
        auto vkDevice = m_device->get_vk_device();

        vkDevice.destroy(m_pipeline);
    }

    void ShaderProgramVulkan::create_shaders(const std::array<ShaderDecl, u8(ShaderType::Count)>& shaderStages)
    {
        auto vkDevice = m_device->get_vk_device();

        for (auto i = 0; i < u8(ShaderType::Count); ++i)
        {
            const auto& shaderStageDecl = shaderStages[i];

            if (!shaderStageDecl.ByteCode.empty())
            {
                vk::ShaderModuleCreateInfo moduleInfo{};
                moduleInfo.setCodeSize(shaderStageDecl.ByteCode.size());
                moduleInfo.setPCode(reinterpret_cast<const uint32_t*>(shaderStageDecl.ByteCode.data()));
                m_modules.push_back(vkDevice.createShaderModule(moduleInfo));

                auto& stageInfo = m_stageInfos.emplace_back();
                stageInfo.setModule(m_modules.back());
                stageInfo.setStage(convert(ShaderType(i)));
                stageInfo.setPName("main");

                ++m_usedStageCount;
            }
        }
    }

}