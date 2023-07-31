#pragma once

#include "rune/common.hpp"
#include "common.hpp"
#include "resource.hpp"

namespace rune::rhi
{
    enum class IndexType
    {
        U16,
        U32,
    };

    class Device;
    class RenderTarget;
    class PipelineState;
    class Buffer;
    class Image;

    class CommandList : public Resource
    {
    public:
        CommandList(Device& device) {}
        virtual ~CommandList() = default;

        virtual void reset() = 0;

        virtual void begin() = 0;
        virtual void end() = 0;

        virtual void begin_render_pass(RenderTarget* renderTarget) = 0;
        virtual void end_renderr_pass() = 0;

        virtual void bind_pipeline_state(PipelineState* pipelineState) = 0;

        virtual void set_viewport(f32 x, f32 y, f32 width, f32 height, f32 minDepth = 0.0f, f32 maxDepth = 1.0f) = 0;
        virtual void set_scissor(i32 x, i32 y, u32 width, u32 height) = 0;

        virtual void bind_vertex_buffers(u32 first, const std::vector<Buffer*>& buffers, const std::vector<u64>& offsets) = 0;
        virtual void bind_index_buffer(Buffer* buffer, u64 offset, IndexType indexType) = 0;

        virtual void draw(u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance) = 0;
        virtual void draw_indexed(u32 indexCount, u32 instanceCount, u32 firstIndex, i32 vertexOffset, u32 firstInstance) = 0;

        virtual void transition_state(Image* image, ResourceState oldState, ResourceState newState) = 0;

        virtual void copy_buffer_to_buffer(Buffer* dstBuffer, u64 dstOffset, Buffer* srcBuffer, u64 srcOffset, u64 size) = 0;
    };
}