#pragma once

#include "rune/common.hpp"

namespace rune::rhi
{
    enum class PrimitiveTopology
    {
        LineList,
        TriangleList,
    };

    struct DepthStencilStateDecl
    {
        bool DepthTestEnable;
        bool DepthWriteEnable;
        // DepthFunc
        bool StencilTestEnable;
        // FrontFace (StencilFailOp, DepthFailOp, PassOp, etc.)
        // BackFace (StencilFailOp, DepthFailOp, PassOp, etc.)
    };

    enum class FillMode
    {
        Solid,
    };

    enum class CullMode
    {
        None,
        Front,
        Back,
        FrontAndBack,
    };

    struct RasterizerStateDecl
    {
        FillMode FillMode;
        CullMode CullMode;
        bool ReverseFrontFace;  // True=Counter-Clockwise

        f32 DepthBias;
        f32 DepthBiasClamp;
        f32 SlopeScaledDepthBias;
        bool DepthClipEnable;
        bool ScissorTestEnable;
        // bool AntialiasedLinesEnabled; ??
    };

    struct BlendStateDecl
    {
    };

    class ShaderProgram;

    struct PipelineStateDecl
    {
        Shared<ShaderProgram> ShaderProgram{ nullptr };
        PrimitiveTopology Topology{};
        DepthStencilStateDecl DepthStencil{};
        RasterizerStateDecl Rasterizer{};
    };

    class Device;

    /* A PipelineState is defined by a ShaderProgram and various pipeline states. Different PipelineStates can use the same ShaderProgram.
     */
    class PipelineState
    {
    public:
        static auto create(Shared<Device> device, const PipelineStateDecl& decl) -> Owned<PipelineState>;

        virtual ~PipelineState() = default;

        virtual auto get_shader_program() const -> Shared<ShaderProgram> = 0;
    };
}