#pragma once

#include "rune/rhi/structs.hpp"
#include "device_vk.hpp"
#include "type_conversions_vk.hpp"

#include <vulkan/vulkan.hpp>
#include <vulkan-memory-allocator-hpp/vk_mem_alloc.hpp>

#include <vector>

namespace rune::rhi
{
    class DeviceInternal;
    struct PipelineState;

    struct CommandListInternal
    {
        std::shared_ptr<DeviceInternal> device = nullptr;
        vk::CommandBuffer cmd = nullptr;
        std::vector<vk::ImageMemoryBarrier2> preRenderPassBarriers = {};
        std::vector<vk::ImageMemoryBarrier2> postRenderPassBarriers = {};

        PipelineState* activePipelineState = nullptr;

        std::vector<std::shared_ptr<SwapchainInternal>> usedSwapchains = {};
        std::vector<std::shared_ptr<void>> usedResources = {};

        CommandListInternal(std::shared_ptr<DeviceInternal> device) : device(device)
        {
            vk::CommandBufferAllocateInfo allocInfo{};
            allocInfo.setCommandBufferCount(1);
            allocInfo.setCommandPool(device->commandPool);
            allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
            cmd = device->device.allocateCommandBuffers(allocInfo)[0];
        }

        ~CommandListInternal() { device->device.freeCommandBuffers(device->commandPool, cmd); }
    };

    struct FenceInternal
    {
        std::shared_ptr<DeviceInternal> device = nullptr;
        vk::Fence fence = nullptr;

        FenceInternal(std::shared_ptr<DeviceInternal> device, bool signaled) : device(device)
        {
            fence = device->device.createFence({ signaled ? vk::FenceCreateFlagBits::eSignaled : vk::FenceCreateFlagBits() });
        }

        ~FenceInternal() { device->device.destroy(fence); }

        void wait() const
        {
            device->device.waitForFences(fence, true, std::uint64_t(-1));
            device->device.resetFences(fence);
        }
    };

    struct SwapchainInternal
    {
        std::shared_ptr<DeviceInternal> device = nullptr;
        vk::SurfaceKHR surface;
        vk::SwapchainKHR swapchain;
        std::uint32_t imageIndex;
        std::vector<vk::Image> images;
        std::vector<vk::ImageView> imageViews;
        vk::Semaphore acquireSemaphore;  // Signalled on acquire complete
        vk::Semaphore releaseSemaphore;  // Signalled on render complete

        SwapchainInternal(std::shared_ptr<DeviceInternal> device, void* window) : device(device)
        {
            if (!surface)
            {
#ifdef _WIN32
                vk::Win32SurfaceCreateInfoKHR surfaceInfo{};
                surfaceInfo.setHinstance(GetModuleHandle(nullptr));
                surfaceInfo.setHwnd(HWND(window));
                surface = device->instanceState->instance.createWin32SurfaceKHR(surfaceInfo);
#else
    #error RUNE RHI Device Error: Platform not supported.
#endif
            }
        }

        ~SwapchainInternal()
        {
            device->device.destroy(acquireSemaphore);
            device->device.destroy(releaseSemaphore);

            for (auto& view : imageViews)
                device->device.destroy(view);
            imageViews.clear();

            device->device.destroy(swapchain);
            device->instanceState->instance.destroy(surface);
        }

        void resize(const SwapChainDesc& desc);
    };

    struct ShaderProgramInternal
    {
        vk::Pipeline pipeline;
        vk::PipelineBindPoint bindPoint;
    };

    struct BufferInternal
    {
        std::shared_ptr<DeviceInternal> device = nullptr;
        vk::Buffer buffer;
        vma::Allocation allocation;

        BufferInternal(std::shared_ptr<DeviceInternal>& device, const BufferDesc& desc) : device(device)
        {
            vk::BufferCreateInfo bufferInfo{};
            bufferInfo.setSize(desc.size);
            bufferInfo.setUsage(convert_buffer(desc.usageFlags));

            vma::AllocationCreateInfo allocInfo{};
            allocInfo.setUsage(convert(desc.readWriteUsage));
            if (desc.readWriteUsage == ReadWriteUsage::Upload || desc.readWriteUsage == ReadWriteUsage::ReadBack)
                allocInfo.setFlags(vma::AllocationCreateFlagBits::eHostAccessSequentialWrite);

            std::tie(buffer, allocation) = device->allocator.createBuffer(bufferInfo, allocInfo);
        }

        ~BufferInternal()
        {
            if (allocation)
                device->allocator.destroyBuffer(buffer, allocation);
        }
    };
}