#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan-memory-allocator-hpp/vk_mem_alloc.hpp>

#include <vector>

namespace rune::rhi
{
    struct VulkanInstance
    {
        vk::DynamicLoader loader;
        vk::Instance instance;
        vk::DebugUtilsMessengerEXT messenger;

        VulkanInstance();
        ~VulkanInstance();
    };

    struct CommandListInternal;
    struct SwapchainInternal;

    struct DeviceInternal
    {
        std::shared_ptr<VulkanInstance> instanceState;
        vk::PhysicalDevice physicalDevice;
        vk::Device device;
        vk::Queue graphicsQueue;
        vma::Allocator allocator;
        vk::CommandPool commandPool;
        std::vector<std::shared_ptr<CommandListInternal>> activeCmdLists;
        std::vector<std::shared_ptr<SwapchainInternal>> activeSwapChains;
        std::vector<vk::Semaphore> submitWaitSemaphores;
        std::vector<vk::Semaphore> submitSignalSemaphores;

        ~DeviceInternal();
    };
}