#include "rune/rhi/device.hpp"

#include "device_vk.hpp"
#include "resources_vk.hpp"
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
    }

    Device::~Device()
    {
        wait_for_gpu();
    }

    bool Device::create_swapchain(const SwapChainDesc& desc, void* window, Swapchain& swapchain)
    {
        if (!swapchain.is_valid())
        {
            auto vkSwapchainState = std::make_shared<SwapchainInternal>(internal, window);
            swapchain.internal = vkSwapchainState;
        }
        swapchain.internal->resize(desc);

        return true;
    }

    bool Device::create_shader_program(const ShaderProgramDesc& desc, ShaderProgram& program)
    {
        program.internal = std::make_shared<ShaderProgramInternal>();
        program.desc = desc;

#define CREATE_SHADER_MODULE(_stageDesc, _vkStage)                                             \
    if (_stageDesc.enabled)                                                                    \
    {                                                                                          \
        vk::ShaderModuleCreateInfo moduleInfo{};                                               \
        moduleInfo.pCode = reinterpret_cast<const std::uint32_t*>(_stageDesc.byteCode.data()); \
        moduleInfo.codeSize = _stageDesc.byteCode.size();                                      \
        auto module = internal->device.createShaderModule(moduleInfo);                         \
                                                                                               \
        vk::PipelineShaderStageCreateInfo stageInfo{};                                         \
        stageInfo.setModule(module);                                                           \
        stageInfo.setPName("main");                                                            \
        stageInfo.setStage(vk::ShaderStageFlagBits::_vkStage);                                 \
        program.internal->stages.push_back(stageInfo);                                         \
    }

        if (desc.stages.compute.enabled)
        {
            CREATE_SHADER_MODULE(desc.stages.compute, eCompute);
            program.internal->bindPoint = vk::PipelineBindPoint::eCompute;
        }
        else
        {
            CREATE_SHADER_MODULE(desc.stages.vertex, eVertex);
            CREATE_SHADER_MODULE(desc.stages.fragment, eFragment);
            program.internal->bindPoint = vk::PipelineBindPoint::eGraphics;
        }

        return true;
    }

    bool Device::create_buffer(const BufferDesc& desc, Buffer& buffer)
    {
        buffer.internal = std::make_shared<BufferInternal>();

        return true;
    }

    auto Device::begin_command_list(QueueType queueType) -> CommandList
    {
        auto vkCmdListInternalState = std::make_shared<CommandListInternal>(internal);

        CommandList cmdList{};
        cmdList.queueType = queueType;
        cmdList.internal = vkCmdListInternalState;

        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        vkCmdListInternalState->cmd.begin(beginInfo);

        internal->activeCmdLists.push_back(vkCmdListInternalState);

        return cmdList;
    }

    void Device::submit_command_lists()
    {
        std::vector<vk::Semaphore> submitSignalSemaphores{};
        if (!internal->activeCmdLists.empty())
        {
            // Command Buffers
            std::vector<vk::CommandBufferSubmitInfo> cmdBufferInfos{};
            for (auto i = 0; i < internal->activeCmdLists.size(); ++i)
            {
                internal->activeCmdLists[i]->cmd.end();

                auto& cmdInfo = cmdBufferInfos.emplace_back();
                cmdInfo.setCommandBuffer(internal->activeCmdLists[i]->cmd);
            }

            // Wait Semaphores
            std::vector<vk::SemaphoreSubmitInfo> waitSemaphoreInfos{};
            for (auto i = 0; i < internal->activeSwapChains.size(); ++i)
            {
                auto& swapchain = internal->activeSwapChains[i];

                auto& semaphoreInfo = waitSemaphoreInfos.emplace_back();
                semaphoreInfo.setSemaphore(swapchain->acquireSemaphore);
                semaphoreInfo.setStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput);
            }

            // Signal Semaphores
            std::vector<vk::SemaphoreSubmitInfo> signalSemaphoreInfos{};
            for (auto i = 0; i < internal->activeSwapChains.size(); ++i)
            {
                auto& swapchain = internal->activeSwapChains[i];

                submitSignalSemaphores.push_back(swapchain->releaseSemaphore);
                auto& semaphoreInfo = signalSemaphoreInfos.emplace_back();
                semaphoreInfo.setSemaphore(swapchain->releaseSemaphore);
            }

            vk::SubmitInfo2 submitInfo{};
            submitInfo.setCommandBufferInfos(cmdBufferInfos);
            submitInfo.setWaitSemaphoreInfos(waitSemaphoreInfos);
            submitInfo.setSignalSemaphoreInfos(signalSemaphoreInfos);

            internal->graphicsQueue.submit2(submitInfo);
            internal->activeCmdLists.clear();
        }

        // Present Swapchains
        if (!internal->activeSwapChains.empty())
        {
            std::vector<vk::SwapchainKHR> swapchains(internal->activeSwapChains.size());
            std::vector<std::uint32_t> imageIndices(internal->activeSwapChains.size());
            for (auto i = 0; i < swapchains.size(); ++i)
            {
                swapchains[i] = internal->activeSwapChains[i]->swapchain;
                imageIndices[i] = internal->activeSwapChains[i]->imageIndex;
            }

            vk::PresentInfoKHR presentInfo{};
            presentInfo.setSwapchains(swapchains);
            presentInfo.setImageIndices(imageIndices);
            presentInfo.setWaitSemaphores(submitSignalSemaphores);
            void(internal->graphicsQueue.presentKHR(presentInfo));
        }
    }

    void Device::wait_for_gpu()
    {
        internal->device.waitIdle();
    }

    void Device::destroy_resource(Buffer& resource)
    {
        resource.internal = nullptr;
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

        internal->submitWaitSemaphores.push_back(swapchain.internal->acquireSemaphore);
        internal->submitSignalSemaphores.push_back(swapchain.internal->releaseSemaphore);

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

        // #TODO: Hash/Get/Create pipeline (base on current render pass)

        cmd.bindPipeline(program->internal->bindPoint, {});

        // cmd.setPrimitiveTopology();

        cmdList.internal->usedResources.push_back(program->internal);
    }

}
