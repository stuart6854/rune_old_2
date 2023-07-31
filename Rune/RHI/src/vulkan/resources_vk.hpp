#pragma once

#include "rune/rhi/structs.hpp"

#include <vulkan/vulkan.hpp>
#include <vulkan-memory-allocator-hpp/vk_mem_alloc.hpp>

#include <vector>

namespace rune::rhi
{
    struct CommandListVulkan
    {
        vk::CommandBuffer cmd;
        std::vector<vk::ImageMemoryBarrier2> preRenderPassBarriers;
        std::vector<vk::ImageMemoryBarrier2> postRenderPassBarriers;
    };

    struct SwapChainVulkan
    {
        vk::Instance instance;
        vk::PhysicalDevice physicalDevice;
        vk::Device device;
        vk::SurfaceKHR surface;
        vk::SwapchainKHR swapchain;
        std::uint32_t imageIndex;
        std::vector<vk::Image> images;
        std::vector<vk::ImageView> imageViews;
        vk::Semaphore acquireSemaphore;  // Signalled on acquire complete
        vk::Semaphore releaseSemaphore;  // Signalled on render complete

        ~SwapChainVulkan();

        void resize(const SwapChainDesc& desc, vk::PhysicalDevice physicalDevice, vk::Device device);
    };

    struct ShaderProgramVulkan
    {
        std::vector<vk::PipelineShaderStageCreateInfo> stages;
        vk::PipelineBindPoint bindPoint;
    };

    struct BufferVulkan
    {
        vma::Allocator allocator;
        vk::Buffer buffer;
        vma::Allocation allocation;

        ~BufferVulkan()
        {
            if (allocation)
                allocator.destroyBuffer(buffer, allocation);
        }
    };
}