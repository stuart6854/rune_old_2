#pragma once

#include "rune/rendering/rhi/shader_program.hpp"

#include <vulkan/vulkan.hpp>

#include <array>

namespace rune::rhi
{
    class DeviceVulkan;

    class ShaderProgramVulkan : public ShaderProgram
    {
    public:
        ShaderProgramVulkan(Shared<DeviceVulkan> device, const ShaderProgramDecl& decl);
        ~ShaderProgramVulkan();

        auto get_vk_bind_point() const -> vk::PipelineBindPoint { return m_bindPoint; }
        auto get_vk_pipeline() const -> vk::Pipeline { return m_pipeline; }

    private:
        void create_shaders(const std::array<ShaderDecl, u8(ShaderType::Count)>& shaderStages);

    private:
        Shared<DeviceVulkan> m_device{ nullptr };

        vk::PipelineBindPoint m_bindPoint{};
        vk::Pipeline m_pipeline{};
        vk::PipelineLayout m_layout{};

        std::vector<vk::ShaderModule> m_modules;
        std::vector<vk::PipelineShaderStageCreateInfo> m_stageInfos{};
        u32 m_usedStageCount{};

        // Description??
        // std::vector<vk::ShaderModule> m_shaderModules{};
        // std::vector<std::string> m_entryPoints;
        // u32 m_stageCount;
        // std::vector<> m_vertexInputFormat;
    };
}