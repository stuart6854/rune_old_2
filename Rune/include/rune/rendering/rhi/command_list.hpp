#pragma once

#include "rune/common.hpp"
#include "common.hpp"

namespace rune::rhi
{
    class Device;
    class RenderTarget;
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

        virtual void bind_pipeline() = 0;

        virtual void draw(u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance) = 0;
        virtual void draw_indexed(u32 indexCount, u32 instanceCount, u32 firstIndex, i32 vertexOffset, u32 firstInstance) = 0;

        virtual void transition_state(Image* image, ResourceState oldState, ResourceState newState) = 0;
    };
}