#pragma once

#include "rune/rendering/rhi/command_list.hpp"

#include <vulkan/vulkan.hpp>

namespace rune::rhi
{
    class PipelineState;
    class DeviceVulkan;
    class PipelineStateVulkan;

    class CommandListVulkan : public CommandList
    {
    public:
        CommandListVulkan(DeviceVulkan& device, bool autoSubmit);
        ~CommandListVulkan();

        auto get_vk_cmd_buffer() const -> vk::CommandBuffer { return m_cmdBuffer; }

        void reset() override;

        void begin() override;
        void end() override;

        void begin_render_pass(RenderTarget* renderTarget) override;
        void end_renderr_pass() override;

        void bind_pipeline_state(PipelineState* pipelineState) override;

        void set_viewport(f32 x, f32 y, f32 width, f32 height, f32 minDepth = 0.0f, f32 maxDepth = 1.0f) override;
        void set_scissor(i32 x, i32 y, u32 width, u32 height) override;

        void bind_vertex_buffers(u32 first, const std::vector<Buffer*>& buffers, const std::vector<u64>& offsets) override;
        void bind_index_buffer(Buffer* buffer, u64 offset, IndexType indexType) override;

        void draw(u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance) override;
        void draw_indexed(u32 indexCount, u32 instanceCount, u32 firstIndex, i32 vertexOffset, u32 firstInstance) override;

        void transition_state(Image* image, ResourceState oldState, ResourceState newState) override;

        void copy_buffer_to_buffer(Buffer* dstBuffer, u64 dstOffset, Buffer* srcBuffer, u64 srcOffset, u64 size) override;

        bool operator==(const CommandListVulkan& rhs) const { return m_cmdBuffer == rhs.m_cmdBuffer; }
        bool operator!=(const CommandListVulkan& rhs) const { return !(*this == rhs); }

    private:
        DeviceVulkan& m_device;
        bool m_autoSubmit;

        vk::CommandBuffer m_cmdBuffer{};

        PipelineStateVulkan* m_boundPipelineState{ nullptr };
    };
}