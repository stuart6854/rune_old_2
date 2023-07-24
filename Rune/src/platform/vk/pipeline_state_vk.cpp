#include "pipeline_state_vk.hpp"

#include "device_vk.hpp"
#include "type_conversions_vk.hpp"

namespace rune::rhi
{
    auto PipelineState::create(Shared<Device> device, const PipelineStateDecl& decl) -> Owned<PipelineState>
    {
        return create_owned<PipelineStateVulkan>(static_pointer_cast<DeviceVulkan>(device), decl);
    }

    PipelineStateVulkan::PipelineStateVulkan(Shared<DeviceVulkan> device, const PipelineStateDecl& decl) : m_device(device), m_decl(decl)
    {
        m_shaderProgram = static_pointer_cast<ShaderProgramVulkan>(decl.ShaderProgram);
    }

    PipelineStateVulkan::~PipelineStateVulkan() {}

    auto PipelineStateVulkan::get_shader_program() const -> Shared<ShaderProgram>
    {
        return m_shaderProgram;
    }

    void PipelineStateVulkan::bind(vk::CommandBuffer& cmd, PipelineStateVulkan* oldPipelineState)
    {
        auto bindPoint = m_shaderProgram->get_vk_bind_point();
        auto pipeline = m_shaderProgram->get_vk_pipeline();

        if (oldPipelineState == nullptr || m_shaderProgram != oldPipelineState->get_shader_program())
        {
            cmd.bindPipeline(bindPoint, pipeline);
        }

        cmd.setPrimitiveTopology(convert(m_decl.Topology));

        cmd.setDepthTestEnable(m_decl.DepthStencil.DepthTestEnable);
        cmd.setDepthWriteEnable(m_decl.DepthStencil.DepthWriteEnable);
        cmd.setDepthCompareOp(vk::CompareOp::eGreaterOrEqual);  // #TODO
        cmd.setStencilTestEnable(m_decl.DepthStencil.StencilTestEnable);
        cmd.setStencilOp(vk::StencilFaceFlagBits::eBack, {}, {}, {}, {});  // #TODO
        //  FrontFace (StencilFailOp, DepthFailOp, PassOp, etc.)
        //  BackFace (StencilFailOp, DepthFailOp, PassOp, etc.)

        // cmd.setFillMode();
        cmd.setCullMode(convert(m_decl.Rasterizer.CullMode));
        cmd.setFrontFace(m_decl.Rasterizer.ReverseFrontFace ? vk::FrontFace::eCounterClockwise
                                                            : vk::FrontFace::eClockwise);  // True=Counter-Clockwise

        cmd.setDepthBiasEnable(m_decl.Rasterizer.DepthBias > 0.0f);
        cmd.setDepthBias(m_decl.Rasterizer.DepthBias, m_decl.Rasterizer.DepthBiasClamp, m_decl.Rasterizer.SlopeScaledDepthBias);
        // cmd.setDepthClipEnableEXT(m_decl.Rasterizer.DepthClipEnable);
        cmd.setStencilTestEnable(m_decl.Rasterizer.ScissorTestEnable);
        //  AntialiasedLinesEnabled
    }

}