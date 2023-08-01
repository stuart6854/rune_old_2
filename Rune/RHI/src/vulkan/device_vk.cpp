#include "rune/rhi/device.hpp"

#include "device_vk.hpp"
#include "resources_vk.hpp"
#include "pipeline_library_vk.hpp"
#include "type_conversions_vk.hpp"

#include <vulkan/vulkan.hpp>
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
#include <vulkan-memory-allocator-hpp/vk_mem_alloc.hpp>

#include <any>
#include <memory>
#include <format>
#include <cassert>
#include <iostream>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace rune::rhi
{
    std::shared_ptr<VulkanInstance> s_vulkanInstanceState{ nullptr };

    VKAPI_ATTR VkBool32 VKAPI_CALL debug_utils_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                                  VkDebugUtilsMessageTypeFlagsEXT /*message_type*/,
                                                                  const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                                                                  void* /*user_data*/)
    {
        if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            std::cout << std::format("{} - {}: {}", callback_data->messageIdNumber, callback_data->pMessageIdName, callback_data->pMessage)
                      << std::endl;
        }
        else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        {
            std::cerr << std::format("{} - {}: {}", callback_data->messageIdNumber, callback_data->pMessageIdName, callback_data->pMessage)
                      << std::endl;
            assert(false);
        }
        return VK_FALSE;
    }

    VulkanInstance::VulkanInstance()
    {
        PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = loader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
        VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

        vk::ApplicationInfo appInfo{};
        appInfo.setPEngineName("RuneEngine");
        appInfo.setEngineVersion(VK_MAKE_API_VERSION(0, 0, 1, 0));
        appInfo.setApiVersion(VK_API_VERSION_1_3);

        std::vector<const char*> extensions{
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,

            VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef _WIN32
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
        };

        std::vector<const char*> layers{
            "VK_LAYER_KHRONOS_validation",
        };

        vk::DebugUtilsMessengerCreateInfoEXT debugMessengerInfo{};
        debugMessengerInfo.setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
                                              vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning);
        debugMessengerInfo.setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
        debugMessengerInfo.setPfnUserCallback(debug_utils_messenger_callback);

        vk::InstanceCreateInfo instanceInfo{};
        instanceInfo.setPApplicationInfo(&appInfo);
        instanceInfo.setPEnabledExtensionNames(extensions);
        instanceInfo.setPEnabledLayerNames(layers);
        instanceInfo.setPNext(&debugMessengerInfo);

        instance = vk::createInstance(instanceInfo);
        if (!instance)
        {
            return;
        }
        VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);

        messenger = instance.createDebugUtilsMessengerEXT(debugMessengerInfo);
        if (!messenger)
        {
            return;
        }
    }

    VulkanInstance::~VulkanInstance()
    {
        instance.destroy(messenger);
        instance.destroy();
    }

    DeviceInternal::~DeviceInternal()
    {
        device.destroy(commandPool);
        allocator.destroy();
        device.destroy();
        instanceState = nullptr;

        if (s_vulkanInstanceState.use_count() == 1)
            s_vulkanInstanceState = nullptr;
    }

    Device::Device(bool useDebugLayer) : internal(std::make_shared<DeviceInternal>())
    {
        if (!s_vulkanInstanceState)
        {
            s_vulkanInstanceState = std::make_shared<VulkanInstance>();
        }

        internal->instanceState = s_vulkanInstanceState;

        // #TODO: Pick best GPU
        internal->physicalDevice = internal->instanceState->instance.enumeratePhysicalDevices()[0];
        if (!internal->physicalDevice)
        {
            internal = nullptr;
            return;
        }

        // #TODO: Find best Graphics(graphics + compute), Compute, Copy queues
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
        internal->device = internal->physicalDevice.createDevice(deviceInfo);
        if (!internal->device)
        {
            internal = nullptr;
            return;
        }
        VULKAN_HPP_DEFAULT_DISPATCHER.init(internal->device);

        internal->graphicsQueue = internal->device.getQueue(0, 0);

        vma::AllocatorCreateInfo allocatorInfo{};
        allocatorInfo.setInstance(internal->instanceState->instance);
        allocatorInfo.setPhysicalDevice(internal->physicalDevice);
        allocatorInfo.setDevice(internal->device);
        allocatorInfo.setVulkanApiVersion(VK_API_VERSION_1_3);
        internal->allocator = vma::createAllocator(allocatorInfo);

        vk::CommandPoolCreateInfo cmdPoolInfo{};
        cmdPoolInfo.setQueueFamilyIndex(0);
        cmdPoolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
        internal->commandPool = internal->device.createCommandPool(cmdPoolInfo);

        internal->pipelineLibrary = std::make_unique<PipelineLibraryVulkan>(internal);
    }

    Device::~Device()
    {
        wait_for_gpu();
    }

    bool Device::create_command_list(QueueType queueType, CommandList& cmdList)
    {
        cmdList.queueType = queueType;
        cmdList.internal = std::make_shared<CommandListInternal>(internal);
        return true;
    }

    bool Device::create_fence(bool signaled, Fence& fence)
    {
        fence.internal = std::make_shared<FenceInternal>(internal, signaled);
        return true;
    }

    bool Device::create_swapchain(const SwapChainDesc& desc, void* window, Swapchain& swapchain)
    {
        if (!swapchain.is_valid())
        {
            auto vkSwapchainState = std::make_shared<SwapchainInternal>(internal, window);
            swapchain.internal = vkSwapchainState;
        }
        swapchain.desc = desc;
        swapchain.internal->resize(desc);

        return true;
    }

    bool Device::create_shader_program(ShaderProgramDesc& desc, ShaderProgram& program)
    {
        program.internal = std::make_shared<ShaderProgramInternal>();
        program.desc = desc;

        program.internal->pipeline = internal->pipelineLibrary->get_or_build(desc);

        return true;
    }

    bool Device::create_buffer(const BufferDesc& desc, Buffer& buffer)
    {
        buffer.internal = std::make_shared<BufferInternal>();

        return true;
    }

    void Device::submit_command_lists(const std::vector<CommandList>& cmdList, Fence& fence)
    {
        std::vector<vk::CommandBuffer> cmdBuffersToSubmit(cmdList.size());
        std::vector<vk::PipelineStageFlags> submitWaitDstStageMasks;
        std::vector<vk::Semaphore> submitWaitSemaphores;
        std::vector<vk::Semaphore> submitSignalSemaphores;
        std::vector<vk::SwapchainKHR> swapchainsToPresent;
        std::vector<std::uint32_t> swapchainsToPresentImageIndices;
        for (auto i = 0; i < cmdList.size(); ++i)
        {
            cmdBuffersToSubmit[i] = { cmdList[i].internal->cmd };

            for (auto& usedSwapchain : cmdList[i].internal->usedSwapchains)
            {
                submitWaitDstStageMasks.push_back(vk::PipelineStageFlagBits::eColorAttachmentOutput);
                submitWaitSemaphores.push_back(usedSwapchain->acquireSemaphore);
                submitSignalSemaphores.push_back(usedSwapchain->releaseSemaphore);
                swapchainsToPresent.push_back(usedSwapchain->swapchain);
                swapchainsToPresentImageIndices.push_back(usedSwapchain->imageIndex);
            }
        }

        vk::SubmitInfo submitInfo{};
        submitInfo.setCommandBuffers(cmdBuffersToSubmit);
        submitInfo.setWaitDstStageMask(submitWaitDstStageMasks);
        submitInfo.setWaitSemaphores(submitWaitSemaphores);
        submitInfo.setSignalSemaphores(submitSignalSemaphores);

        internal->graphicsQueue.submit(submitInfo, fence.internal->fence);

        // Present Swapchains
        if (!swapchainsToPresent.empty())
        {
            vk::PresentInfoKHR presentInfo{};
            presentInfo.setSwapchains(swapchainsToPresent);
            presentInfo.setImageIndices(swapchainsToPresentImageIndices);
            presentInfo.setWaitSemaphores(submitSignalSemaphores);
            auto result = internal->graphicsQueue.presentKHR(presentInfo);
            assert(result == vk::Result::eSuccess);
        }
    }

    void Device::wait_for_fence(Fence& fence)
    {
        fence.internal->wait();
    }

    void Device::wait_for_gpu()
    {
        internal->device.waitIdle();
    }

    void Device::destroy_resource(Buffer& resource)
    {
        resource.internal = nullptr;
    }

    void Device::begin(CommandList& cmdList)
    {
        cmdList.internal->activePipelineState = nullptr;
        cmdList.internal->preRenderPassBarriers.clear();
        cmdList.internal->postRenderPassBarriers.clear();
        cmdList.internal->usedSwapchains.clear();
        cmdList.internal->usedResources.clear();

        vk::CommandBufferBeginInfo beginInfo{};
        cmdList.internal->cmd.begin(beginInfo);
    }

    void Device::end(CommandList& cmdList)
    {
        cmdList.internal->cmd.end();
    }

    void Device::begin_render_pass(Swapchain& swapchain, CommandList& cmdList)
    {
        auto cmd = cmdList.internal->cmd;

        auto result = internal->device.acquireNextImageKHR(
            swapchain.internal->swapchain, std::uint64_t(-1), swapchain.internal->acquireSemaphore, {});
        swapchain.internal->imageIndex = result.value;

        if (result.result != vk::Result::eSuccess)
        {
            // #TODO: Handle out-of-date swapchain
            // assert(result.result == vk::Result::eSuccess);
            // return;
        }

        vk::ImageMemoryBarrier2 barrier{};
        barrier.setImage(swapchain.internal->images[swapchain.internal->imageIndex]);
        barrier.setOldLayout(vk::ImageLayout::eUndefined);
        barrier.setNewLayout(vk::ImageLayout::eColorAttachmentOptimal);
        barrier.setSrcStageMask(vk::PipelineStageFlagBits2::eTopOfPipe);
        barrier.setDstStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput);
        barrier.setSrcAccessMask(vk::AccessFlagBits2::eNone);
        barrier.setDstAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite);
        barrier.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
        barrier.subresourceRange.setLayerCount(1);
        barrier.subresourceRange.setBaseArrayLayer(0);
        barrier.subresourceRange.setLevelCount(1);
        barrier.subresourceRange.setBaseMipLevel(0);
        cmdList.internal->preRenderPassBarriers.push_back(barrier);

        barrier.setOldLayout(vk::ImageLayout::eColorAttachmentOptimal);
        barrier.setNewLayout(vk::ImageLayout::ePresentSrcKHR);
        barrier.setSrcStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput);
        barrier.setDstStageMask(vk::PipelineStageFlagBits2::eBottomOfPipe);
        barrier.setSrcAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite);
        barrier.setDstAccessMask(vk::AccessFlagBits2::eNone);
        cmdList.internal->postRenderPassBarriers.push_back(barrier);

        vk::DependencyInfo depInfo{};
        depInfo.setImageMemoryBarriers(cmdList.internal->preRenderPassBarriers);
        cmd.pipelineBarrier2(depInfo);
        cmdList.internal->preRenderPassBarriers.clear();

        vk::RenderingAttachmentInfo attachment{};
        attachment.setImageView(swapchain.internal->imageViews[swapchain.internal->imageIndex]);
        attachment.setImageLayout(vk::ImageLayout::eColorAttachmentOptimal);
        attachment.setLoadOp(vk::AttachmentLoadOp::eClear);
        attachment.setStoreOp(vk::AttachmentStoreOp::eStore);
        attachment.clearValue.color.float32[0] = swapchain.desc.clearColor[0];
        attachment.clearValue.color.float32[1] = swapchain.desc.clearColor[1];
        attachment.clearValue.color.float32[2] = swapchain.desc.clearColor[2];
        attachment.clearValue.color.float32[3] = swapchain.desc.clearColor[3];

        vk::RenderingInfo renderingInfo{};
        renderingInfo.setRenderArea({ { 0, 0 }, { swapchain.desc.width, swapchain.desc.height } });
        renderingInfo.setLayerCount(1);
        renderingInfo.setColorAttachments(attachment);

        cmd.beginRendering(renderingInfo);

        cmdList.internal->usedSwapchains.push_back(swapchain.internal);
        cmdList.internal->usedResources.push_back(swapchain.internal);
    }

    void Device::end_render_pass(CommandList& cmdList)
    {
        auto cmd = cmdList.internal->cmd;

        cmdList.internal->cmd.endRendering();

        vk::DependencyInfo depInfo{};
        depInfo.setImageMemoryBarriers(cmdList.internal->postRenderPassBarriers);
        cmd.pipelineBarrier2(depInfo);

        cmdList.internal->postRenderPassBarriers.clear();
    }

    void Device::set_pipeline_state(PipelineState& state, CommandList& cmdList)
    {
        auto cmd = cmdList.internal->cmd;

        assert(state.program);
        if (state.program == nullptr)
        {
            // #TODO: Error
            return;
        }
        auto& program = state.program;
        auto* oldPipelineState = cmdList.internal->activePipelineState;

        if (oldPipelineState == nullptr || oldPipelineState->program != program)
            cmd.bindPipeline(program->internal->bindPoint, program->internal->pipeline);

        if (oldPipelineState == nullptr || oldPipelineState->topology != state.topology)
            cmd.setPrimitiveTopology(convert(state.topology));

        if (oldPipelineState == nullptr || oldPipelineState->rasterization.cullMode != state.rasterization.cullMode)
            cmd.setCullMode(convert(state.rasterization.cullMode));

        if (oldPipelineState == nullptr || oldPipelineState->rasterization.reverseFrontFace != state.rasterization.reverseFrontFace)
            cmd.setFrontFace(state.rasterization.reverseFrontFace ? vk::FrontFace::eCounterClockwise : vk::FrontFace::eClockwise);

        if (oldPipelineState == nullptr || oldPipelineState->depthStencil.depthTestEnable != state.depthStencil.depthTestEnable)
            cmd.setDepthTestEnable(state.depthStencil.depthTestEnable);

        if (oldPipelineState == nullptr || oldPipelineState->depthStencil.depthWriteEnable != state.depthStencil.depthWriteEnable)
            cmd.setDepthWriteEnable(state.depthStencil.depthWriteEnable);

        if (oldPipelineState == nullptr || oldPipelineState->depthStencil.stencilTestEnable != state.depthStencil.stencilTestEnable)
            cmd.setStencilTestEnable(state.depthStencil.stencilTestEnable);

        cmdList.internal->activePipelineState = &state;
        cmdList.internal->usedResources.push_back(program->internal);
    }

    void Device::set_viewport(
        float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth, CommandList& cmdList)
    {
        auto cmd = cmdList.internal->cmd;

        vk::Viewport viewport{};
        viewport.x = topLeftX;
        viewport.y = topLeftY + height;
        viewport.width = width;
        viewport.height = -height;
        viewport.minDepth = minDepth;
        viewport.maxDepth = maxDepth;
        cmd.setViewport(0, viewport);
    }

    void Device::set_scissor(std::int32_t left, std::int32_t top, std::int32_t right, std::int32_t bottom, CommandList& cmdList)
    {
        auto cmd = cmdList.internal->cmd;

        vk::Rect2D scissor{};
        scissor.extent.width = std::abs(right - left);
        scissor.extent.height = std::abs(top - bottom);
        scissor.offset.x = std::max(0, left);
        scissor.offset.y = std::max(0, top);
        cmd.setScissor(0, scissor);
    }

    void Device::draw(std::uint32_t vertexCount, std::uint32_t firstVertex, CommandList& cmdList)
    {
        auto cmd = cmdList.internal->cmd;

        cmd.draw(vertexCount, 1, firstVertex, 0);
    }

    void Device::draw_indexed(std::uint32_t indexCount, std::uint32_t firstIndex, std::uint32_t vertexOffset, CommandList& cmdList)
    {
        auto cmd = cmdList.internal->cmd;

        cmd.drawIndexed(indexCount, 1, firstIndex, vertexOffset, 0);
    }

}
