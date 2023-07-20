#include "command_list_vk.hpp"

#include "device_vk.hpp"
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

    void CommandListVulkan::bind_pipeline()
    {
        m_cmdBuffer.bindPipeline({}, {});
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