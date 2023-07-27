#include "device_vk.hpp"

#include "rune/debug/log_engine.hpp"
#include "rune/debug/assert_engine.hpp"
#include "instance_vk.hpp"
#include "buffer_vk.hpp"
#include "command_list_vk.hpp"
#include "fence_vk.hpp"

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
#include <vulkan-memory-allocator-hpp/vk_mem_alloc.hpp>

#include <vector>
#include <ranges>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace rune::rhi
{
    static Shared<InstanceVulkan> s_instance{};

    VKAPI_ATTR VkBool32 VKAPI_CALL debug_utils_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                                  VkDebugUtilsMessageTypeFlagsEXT message_type,
                                                                  const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                                                                  void* user_data);

    auto Device::create() -> Owned<Device>
    {
        auto device = create_owned<DeviceVulkan>();
        if (!device->initialise())
            return nullptr;

        return std::move(device);
    }

    bool DeviceVulkan::initialise()
    {
        if (!s_instance)
        {
            s_instance = create_owned<InstanceVulkan>();
            if (!s_instance->initialise())
            {
                s_instance = nullptr;
                return false;
            }
        }

        m_instance = s_instance;

        m_physicalDevice = m_instance->get_best_vk_physical_device();
        if (!m_physicalDevice)
        {
            destroy();
            return false;
        }

        std::vector<float> QueuePriorities(16, 1.0f);
        std::vector<vk::DeviceQueueCreateInfo> queueInfos(1);
        queueInfos[0].setQueueFamilyIndex(0);
        queueInfos[0].setQueueCount(1);
        queueInfos[0].setQueuePriorities(QueuePriorities);

        std::vector<const char*> deviceExtensions{
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
            VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME,
            VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
        };

        vk::PhysicalDeviceDynamicRenderingFeatures dynRenderFeatures{};
        dynRenderFeatures.setDynamicRendering(true);

        vk::PhysicalDeviceTimelineSemaphoreFeatures timelineFeatures{};
        timelineFeatures.setTimelineSemaphore(true);
        timelineFeatures.setPNext(&dynRenderFeatures);

        vk::PhysicalDeviceSynchronization2Features sync2Features{};
        sync2Features.setSynchronization2(true);
        sync2Features.setPNext(&timelineFeatures);

        vk::PhysicalDeviceFeatures features{};
        features.fillModeNonSolid = true;
        features.wideLines = true;

        vk::DeviceCreateInfo deviceInfo{};
        deviceInfo.setQueueCreateInfos(queueInfos);
        deviceInfo.setPEnabledExtensionNames(deviceExtensions);
        deviceInfo.setPEnabledFeatures(&features);
        deviceInfo.setPNext(&sync2Features);
        m_device = m_physicalDevice.createDevice(deviceInfo);
        if (!m_device)
        {
            destroy();
            return false;
        }
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_device);

        m_graphicsQueue = m_device.getQueue(0, 0);

        vma::AllocatorCreateInfo allocatorInfo{};
        allocatorInfo.setInstance(m_instance->get_vk_instance());
        allocatorInfo.setPhysicalDevice(m_physicalDevice);
        allocatorInfo.setDevice(m_device);
        allocatorInfo.setVulkanApiVersion(VK_API_VERSION_1_3);
        m_allocator = vma::createAllocator(allocatorInfo);

        vk::CommandPoolCreateInfo cmdPoolInfo{};
        cmdPoolInfo.setQueueFamilyIndex(0);
        cmdPoolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
        m_cmdPool = m_device.createCommandPool(cmdPoolInfo);

        auto props = m_physicalDevice.getProperties();
        std::string deviceName = props.deviceName;
        glm::ivec3 driverVersion = {
            VK_API_VERSION_MAJOR(props.driverVersion),
            VK_API_VERSION_MINOR(props.driverVersion),
            VK_API_VERSION_PATCH(props.driverVersion),
        };
        RUNE_ENG_INFO("Device: {}, Driver: {}.{}.{}", deviceName, driverVersion.x, driverVersion.y, driverVersion.z);

        return true;
    }

    bool DeviceVulkan::destroy()
    {
        m_allocator.destroy();
        m_allocator = nullptr;

        m_device.destroy();
        m_device = nullptr;

        m_instance = nullptr;

        if (s_instance.use_count() == 1)
        {
            s_instance->destroy();
            s_instance = nullptr;
        }

        return true;
    }

    auto DeviceVulkan::get_vk_instance() const -> vk::Instance
    {
        return m_instance->get_vk_instance();
    }

    auto DeviceVulkan::create_cmd_list(bool autoSubmit) -> Owned<CommandList>
    {
        return create_owned<CommandListVulkan>(*this, autoSubmit);
    }

    void DeviceVulkan::submit(Fence* fence, u64 fenceValue)
    {
        vk::Semaphore signalSemaphore{};
        if (fence)
            signalSemaphore = static_cast<FenceVulkan*>(fence)->get_vk_semaphore();

        vk::TimelineSemaphoreSubmitInfo timelineInfo{};
        timelineInfo.setSignalSemaphoreValues(fenceValue);

        vk::SubmitInfo submitInfo{};
        submitInfo.setCommandBuffers(m_cmdBufferSubmissionOrder);
        if (signalSemaphore)
        {
            submitInfo.setSignalSemaphores(signalSemaphore);
            submitInfo.setPNext(&timelineInfo);
        }

        m_graphicsQueue.submit(submitInfo);
        m_cmdBufferSubmissionOrder.clear();
    }

    void DeviceVulkan::submit_single(CommandList& cmdList, Fence* fence, u64 fenceValue)
    {
        vk::Semaphore signalSemaphore{};
        if (fence)
            signalSemaphore = static_cast<FenceVulkan*>(fence)->get_vk_semaphore();

        vk::TimelineSemaphoreSubmitInfo timelineInfo{};
        timelineInfo.setSignalSemaphoreValues(fenceValue);

        auto vkCmdBuffer = static_cast<CommandListVulkan&>(cmdList).get_vk_cmd_buffer();

        vk::SubmitInfo submitInfo{};
        submitInfo.setCommandBuffers(vkCmdBuffer);
        if (signalSemaphore)
        {
            submitInfo.setSignalSemaphores(signalSemaphore);
            submitInfo.setPNext(&timelineInfo);
        }

        m_graphicsQueue.submit(submitInfo);
    }

    void DeviceVulkan::on_cmd_list_reset(CommandList& cmdList)
    {
        // #TODO: Make thread-safe
        auto& vkCmdList = static_cast<CommandListVulkan&>(cmdList);
        auto vkCmdBuffer = vkCmdList.get_vk_cmd_buffer();

        auto it = std::remove(m_cmdBufferSubmissionOrder.begin(), m_cmdBufferSubmissionOrder.end(), vkCmdBuffer);
        if (it != m_cmdBufferSubmissionOrder.end())
            m_cmdBufferSubmissionOrder.erase(it);
    }

    void DeviceVulkan::on_cmd_list_begin(CommandList& cmdList)
    {
        // #TODO: Make thread-safe
        auto& vkCmdList = static_cast<CommandListVulkan&>(cmdList);
        auto vkCmdBuffer = vkCmdList.get_vk_cmd_buffer();

        m_cmdBufferSubmissionOrder.push_back(vkCmdBuffer);
    }

}