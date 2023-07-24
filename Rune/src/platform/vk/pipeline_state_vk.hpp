#pragma once

#include "rune/rendering/rhi/pipeline_state.hpp"
#include "shader_program_vk.hpp"

namespace rune::rhi
{
    class DeviceVulkan;

    class PipelineStateVulkan : public PipelineState
    {
    public:
        PipelineStateVulkan(Shared<DeviceVulkan> device, const PipelineStateDecl& decl);
        ~PipelineStateVulkan();

        void bind(vk::CommandBuffer& cmd, PipelineStateVulkan* oldPipelineState);

        auto get_shader_program() const -> Shared<ShaderProgram> override;

    private:
        Shared<DeviceVulkan> m_device{ nullptr };
        Shared<ShaderProgramVulkan> m_shaderProgram{ nullptr };
        PipelineStateDecl m_decl{};
    };
}