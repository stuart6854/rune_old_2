#pragma once

#include "rune/common.hpp"
#include "common.hpp"

namespace rune::rhi
{
    class Device;
    class RenderTarget;
    class PipelineState;
    class Image;

    class CommandList
    {
    public:
        static auto create(Shared<Device> device) -> Owned<CommandList>;

        virtual void reset() = 0;

        virtual void begin() = 0;
        virtual void end() = 0;

        virtual void begin_render_pass(RenderTarget* renderTarget) = 0;
        virtual void end_renderr_pass() = 0;

        virtual void bind_pipeline_state(PipelineState* pipelineState) = 0;

        virtual void set_viewport(f32 x, f32 y, f32 width, f32 height, f32 minDepth = 0.0f, f32 maxDepth = 1.0f) = 0;
        virtual void set_scissor(i32 x, i32 y, u32 width, u32 height) = 0;

        virtual void draw(u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance) = 0;
        virtual void draw_indexed(u32 indexCount, u32 instanceCount, u32 firstIndex, i32 vertexOffset, u32 firstInstance) = 0;

        virtual void transition_state(Image* image, ResourceState oldState, ResourceState newState) = 0;
    };
}