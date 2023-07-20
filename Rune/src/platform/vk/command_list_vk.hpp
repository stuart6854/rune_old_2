#pragma once

#include "rune/rendering/rhi/command_list.hpp"

#include <vulkan/vulkan.hpp>

namespace rune::rhi
{
    class DeviceVulkan;

    class CommandListVulkan : public CommandList
    {
    public:
        CommandListVulkan(Shared<DeviceVulkan> device);
        ~CommandListVulkan();

        auto get_vk_cmd_buffer() const -> vk::CommandBuffer { return m_cmdBuffer; }

        void reset() override;

        void begin() override;
        void end() override;

        void bind_pipeline() override;

        void draw(u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance) override;
        void draw_indexed(u32 indexCount, u32 instanceCount, u32 firstIndex, i32 vertexOffset, u32 firstInstance) override;

        void transition_state(Image* image, ResourceState oldState, ResourceState newState) override;

    private:
        Shared<DeviceVulkan> m_device{ nullptr };

        vk::CommandBuffer m_cmdBuffer{};
        vk::Fence m_fence{};
    };
}