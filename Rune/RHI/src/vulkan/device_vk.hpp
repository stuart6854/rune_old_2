#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan-memory-allocator-hpp/vk_mem_alloc.hpp>

#include <vector>

namespace rune::rhi
{
    struct VulkanInstanceState
    {
        vk::DynamicLoader loader;
        vk::Instance instance;
        vk::DebugUtilsMessengerEXT messenger;

        VulkanInstanceState();
        ~VulkanInstanceState();
    };

    class CommandListVulkan;
    class SwapChainVulkan;

    struct DeviceVulkanInternalState
    {
        std::shared_ptr<VulkanInstanceState> instanceState;
        vk::PhysicalDevice physicalDevice;
        vk::Device device;
        vk::Queue graphicsQueue;
        vma::Allocator allocator;
        vk::CommandPool commandPool;
        std::vector<std::shared_ptr<CommandListVulkan>> activeCmdLists;
        std::vector<std::shared_ptr<SwapChainVulkan>> activeSwapChains;
        std::vector<vk::Semaphore> submitWaitSemaphores;
        std::vector<vk::Semaphore> submitSignalSemaphores;

        ~DeviceVulkanInternalState();
    };
}