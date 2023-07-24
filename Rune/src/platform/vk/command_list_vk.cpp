#include "command_list_vk.hpp"

#include "device_vk.hpp"
#include "render_target_vk.hpp"
#include "pipeline_state_vk.hpp"
#include "image_vk.hpp"
#include "type_conversions_vk.hpp"

namespace rune::rhi
{
    auto CommandList::create(Shared<Device> device) -> Owned<CommandList>
    {
        return create_owned<CommandListVulkan>(static_pointer_cast<DeviceVulkan>(device));
    }

    CommandListVulkan::CommandListVulkan(Shared<DeviceVulkan> device) : m_device(device)
    {
        auto vkDevice = m_device->get_vk_device();
        auto vkCmdPool = m_device->get_vk_cmd_pool();

        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.setCommandPool(vkCmdPool);
        allocInfo.setCommandBufferCount(1);
        allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
        m_cmdBuffer = vkDevice.allocateCommandBuffers(allocInfo)[0];
    }

    CommandListVulkan::~CommandListVulkan()
    {
        auto vkDevice = m_device->get_vk_device();
        auto vkCmdPool = m_device->get_vk_cmd_pool();

        RUNE_UNUSED(vkDevice.waitForFences(m_fence, true, u64(-1)));
        vkDevice.freeCommandBuffers(vkCmdPool, m_cmdBuffer);
    }

    void CommandListVulkan::reset()
    {
        m_cmdBuffer.reset();
        m_boundPipelineState = nullptr;
    }

    void CommandListVulkan::begin()
    {
        vk::CommandBufferBeginInfo beginInfo{};
        m_cmdBuffer.begin(beginInfo);
    }

    void CommandListVulkan::end()
    {
        m_cmdBuffer.end();
    }

    void CommandListVulkan::begin_render_pass(RenderTarget* renderTarget)
    {
        auto vkRenderTarget = static_cast<RenderTargetVulkan*>(renderTarget);

        m_cmdBuffer.beginRendering(vkRenderTarget->get_vk_rendering_info());
    }

    void CommandListVulkan::end_renderr_pass()
    {
        m_cmdBuffer.endRendering();
    }

    void CommandListVulkan::bind_pipeline_state(PipelineState* pipelineState)
    {
        auto* vkPipelineState = static_cast<PipelineStateVulkan*>(pipelineState);
        vkPipelineState->bind(m_cmdBuffer, m_boundPipelineState);
        m_boundPipelineState = vkPipelineState;
    }

    void CommandListVulkan::set_viewport(f32 x, f32 y, f32 width, f32 height, f32 minDepth, f32 maxDepth)
    {
        vk::Viewport viewport{ x, y, width, height, minDepth, maxDepth };
        m_cmdBuffer.setViewport(0, viewport);
    }

    void CommandListVulkan::set_scissor(i32 x, i32 y, u32 width, u32 height)
    {
        vk::Rect2D scissor{ { x, y }, { width, height } };
        m_cmdBuffer.setScissor(0, scissor);
    }

    void CommandListVulkan::draw(u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance)
    {
        m_cmdBuffer.draw(vertexCount, instanceCount, firstVertex, firstInstance);
    }

    void CommandListVulkan::draw_indexed(u32 indexCount, u32 instanceCount, u32 firstIndex, i32 vertexOffset, u32 firstInstance)
    {
        m_cmdBuffer.drawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }

    void CommandListVulkan::transition_state(Image* image, ResourceState oldState, ResourceState newState)
    {
        auto* vkImage = static_cast<ImageVulkan*>(image);

        auto oldLayout = convert_to_layout(oldState);
        auto newLayout = convert_to_layout(newState);
        auto srcStage = convert_to_stage(oldState);
        auto dstStage = convert_to_stage(newState);
        auto srcAccess = convert_to_access(oldState);
        auto dstAccess = convert_to_access(newState);

        vk::ImageMemoryBarrier2 barrier{};
        barrier.setImage(vkImage->get_vk_image());
        barrier.setOldLayout(oldLayout);
        barrier.setNewLayout(newLayout);
        barrier.setSrcStageMask(srcStage);
        barrier.setDstStageMask(dstStage);
        barrier.setSrcAccessMask(srcAccess);
        barrier.setDstAccessMask(dstAccess);
        barrier.setSubresourceRange(vkImage->get_vk_subresource_range());

        vk::DependencyInfo depInfo{};
        depInfo.setImageMemoryBarriers(barrier);
        m_cmdBuffer.pipelineBarrier2(depInfo);
    }
}