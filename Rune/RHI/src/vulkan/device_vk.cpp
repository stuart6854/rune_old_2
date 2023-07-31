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
    std::shared_ptr<VulkanInstanceState> s_vulkanInstanceState{ nullptr };

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

    VulkanInstanceState::VulkanInstanceState()
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

    VulkanInstanceState::~VulkanInstanceState()
    {
        instance.destroy(messenger);
        instance.destroy();
    }

#define GET_VK_DEVICE_STATE() std::any_cast<DeviceVulkanInternalState>(&m_internalState)

    DeviceVulkanInternalState::~DeviceVulkanInternalState()
    {
        device.destroy(commandPool);
        allocator.destroy();
        device.destroy();
        instanceState = nullptr;

        if (s_vulkanInstanceState.use_count() == 1)
            s_vulkanInstanceState = nullptr;
    }

    Device::Device(bool useDebugLayer) : m_internalState(std::make_any<DeviceVulkanInternalState>())
    {
        auto* vkDeviceInternalState = GET_VK_DEVICE_STATE();

        if (!s_vulkanInstanceState)
        {
            s_vulkanInstanceState = std::make_shared<VulkanInstanceState>();
        }

        vkDeviceInternalState->instanceState = s_vulkanInstanceState;

        // #TODO: Pick best GPU
        vkDeviceInternalState->physicalDevice = vkDeviceInternalState->instanceState->instance.enumeratePhysicalDevices()[0];
        if (!vkDeviceInternalState->physicalDevice)
        {
            m_internalState = nullptr;
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
        vkDeviceInternalState->device = vkDeviceInternalState->physicalDevice.createDevice(deviceInfo);
        if (!vkDeviceInternalState->device)
        {
            m_internalState = nullptr;
            return;
        }
        VULKAN_HPP_DEFAULT_DISPATCHER.init(vkDeviceInternalState->device);

        vkDeviceInternalState->graphicsQueue = vkDeviceInternalState->device.getQueue(0, 0);

        vma::AllocatorCreateInfo allocatorInfo{};
        allocatorInfo.setInstance(vkDeviceInternalState->instanceState->instance);
        allocatorInfo.setPhysicalDevice(vkDeviceInternalState->physicalDevice);
        allocatorInfo.setDevice(vkDeviceInternalState->device);
        allocatorInfo.setVulkanApiVersion(VK_API_VERSION_1_3);
        vkDeviceInternalState->allocator = vma::createAllocator(allocatorInfo);

        vk::CommandPoolCreateInfo cmdPoolInfo{};
        cmdPoolInfo.setQueueFamilyIndex(0);
        cmdPoolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
        vkDeviceInternalState->commandPool = vkDeviceInternalState->device.createCommandPool(cmdPoolInfo);
    }

    Device::~Device()
    {
        wait_for_gpu();
    }

    bool Device::create_swapchain(const SwapChainDesc& desc, void* window, Swapchain& swapchain)
    {
        auto* vkDeviceInternalState = GET_VK_DEVICE_STATE();

        if (!swapchain.is_valid())
        {
            auto newSwapChainState = std::make_shared<SwapChainVulkan>();
            newSwapChainState->instance = vkDeviceInternalState->instanceState->instance;
            newSwapChainState->physicalDevice = vkDeviceInternalState->physicalDevice;
            newSwapChainState->device = vkDeviceInternalState->device;

            swapchain.set_internal_state(newSwapChainState);
            vkDeviceInternalState->activeSwapChains.push_back(newSwapChainState);
        }
        auto vkSwapChainInternalState = swapchain.get_internal_state<SwapChainVulkan>();

        swapchain.desc = desc;
        if (!vkSwapChainInternalState->surface)
        {
#ifdef _WIN32
            vk::Win32SurfaceCreateInfoKHR surfaceInfo{};
            surfaceInfo.setHinstance(GetModuleHandle(nullptr));
            surfaceInfo.setHwnd(HWND(window));
            vkSwapChainInternalState->surface = vkDeviceInternalState->instanceState->instance.createWin32SurfaceKHR(surfaceInfo);
#else
    #error RUNE RHI Device Error: Platform not supported.
#endif
        }
        vkSwapChainInternalState->resize(desc, vkDeviceInternalState->physicalDevice, vkDeviceInternalState->device);

        return true;
    }

    bool Device::create_shader_program(const ShaderProgramDesc& desc, ShaderProgram& program)
    {
        auto* vkDeviceInternalState = GET_VK_DEVICE_STATE();

        auto vkShaderProgramInternalState = std::make_shared<ShaderProgramVulkan>();
        program.set_internal_state(vkShaderProgramInternalState);
        program.desc = desc;

#define CREATE_SHADER_MODULE(_stageDesc, _vkStage)                                             \
    if (_stageDesc.enabled)                                                                    \
    {                                                                                          \
        vk::ShaderModuleCreateInfo moduleInfo{};                                               \
        moduleInfo.pCode = reinterpret_cast<const std::uint32_t*>(_stageDesc.byteCode.data()); \
        moduleInfo.codeSize = _stageDesc.byteCode.size();                                      \
        auto module = vkDeviceInternalState->device.createShaderModule(moduleInfo);            \
                                                                                               \
        vk::PipelineShaderStageCreateInfo stageInfo{};                                         \
        stageInfo.setModule(module);                                                           \
        stageInfo.setPName("main");                                                            \
        stageInfo.setStage(vk::ShaderStageFlagBits::_vkStage);                                 \
        vkShaderProgramInternalState->stages.push_back(stageInfo);                             \
    }

        if (desc.stages.compute.enabled)
        {
            CREATE_SHADER_MODULE(desc.stages.compute, eCompute);
            vkShaderProgramInternalState->bindPoint = vk::PipelineBindPoint::eCompute;
        }
        else
        {
            CREATE_SHADER_MODULE(desc.stages.vertex, eVertex);
            CREATE_SHADER_MODULE(desc.stages.fragment, eFragment);
            vkShaderProgramInternalState->bindPoint = vk::PipelineBindPoint::eGraphics;
        }

        return true;
    }

    bool Device::create_buffer(const BufferDesc& desc, Buffer& buffer)
    {
        auto vkBufferInternalState = std::make_shared<BufferVulkan>();
        buffer.set_internal_state(vkBufferInternalState);

        return true;
    }

    auto Device::begin_command_list(QueueType queueType) -> CommandList
    {
        auto* vkDeviceInternalState = GET_VK_DEVICE_STATE();

        auto vkCmdListInternalState = std::make_shared<CommandListVulkan>();

        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.setCommandBufferCount(1);
        allocInfo.setCommandPool(vkDeviceInternalState->commandPool);
        allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
        vkCmdListInternalState->cmd = vkDeviceInternalState->device.allocateCommandBuffers(allocInfo)[0];

        CommandList cmdList{};
        cmdList.queueType = queueType;
        cmdList.set_internal_state(vkCmdListInternalState);

        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        vkCmdListInternalState->cmd.begin(beginInfo);

        vkDeviceInternalState->activeCmdLists.push_back(vkCmdListInternalState);

        return cmdList;
    }

    void Device::submit_command_lists()
    {
        auto* vkDeviceInternalState = GET_VK_DEVICE_STATE();

        std::vector<vk::Semaphore> submitSignalSemaphores{};
        if (!vkDeviceInternalState->activeCmdLists.empty())
        {
            // Command Buffers
            std::vector<vk::CommandBufferSubmitInfo> cmdBufferInfos{};
            for (auto i = 0; i < vkDeviceInternalState->activeCmdLists.size(); ++i)
            {
                vkDeviceInternalState->activeCmdLists[i]->cmd.end();

                auto& cmdInfo = cmdBufferInfos.emplace_back();
                cmdInfo.setCommandBuffer(vkDeviceInternalState->activeCmdLists[i]->cmd);
            }

            // Wait Semaphores
            std::vector<vk::SemaphoreSubmitInfo> waitSemaphoreInfos{};
            for (auto i = 0; i < vkDeviceInternalState->activeSwapChains.size(); ++i)
            {
                auto& swapchain = vkDeviceInternalState->activeSwapChains[i];

                auto& semaphoreInfo = waitSemaphoreInfos.emplace_back();
                semaphoreInfo.setSemaphore(swapchain->acquireSemaphore);
                semaphoreInfo.setStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput);
            }

            // Signal Semaphores
            std::vector<vk::SemaphoreSubmitInfo> signalSemaphoreInfos{};
            for (auto i = 0; i < vkDeviceInternalState->activeSwapChains.size(); ++i)
            {
                auto& swapchain = vkDeviceInternalState->activeSwapChains[i];

                submitSignalSemaphores.push_back(swapchain->releaseSemaphore);
                auto& semaphoreInfo = signalSemaphoreInfos.emplace_back();
                semaphoreInfo.setSemaphore(swapchain->releaseSemaphore);
            }

            vk::SubmitInfo2 submitInfo{};
            submitInfo.setCommandBufferInfos(cmdBufferInfos);
            submitInfo.setWaitSemaphoreInfos(waitSemaphoreInfos);
            submitInfo.setSignalSemaphoreInfos(signalSemaphoreInfos);

            vkDeviceInternalState->graphicsQueue.submit2(submitInfo);
            vkDeviceInternalState->activeCmdLists.clear();
        }

        // Present Swapchains
        if (!vkDeviceInternalState->activeSwapChains.empty())
        {
            std::vector<vk::SwapchainKHR> swapchains(vkDeviceInternalState->activeSwapChains.size());
            std::vector<std::uint32_t> imageIndices(vkDeviceInternalState->activeSwapChains.size());
            for (auto i = 0; i < swapchains.size(); ++i)
            {
                swapchains[i] = vkDeviceInternalState->activeSwapChains[i]->swapchain;
                imageIndices[i] = vkDeviceInternalState->activeSwapChains[i]->imageIndex;
            }

            vk::PresentInfoKHR presentInfo{};
            presentInfo.setSwapchains(swapchains);
            presentInfo.setImageIndices(imageIndices);
            presentInfo.setWaitSemaphores(submitSignalSemaphores);
            void(vkDeviceInternalState->graphicsQueue.presentKHR(presentInfo));
        }
    }

    void Device::wait_for_gpu()
    {
        auto* vkDeviceInternalState = GET_VK_DEVICE_STATE();
        vkDeviceInternalState->device.waitIdle();
    }

    void Device::defer_deletion(Buffer& resource)
    {
        auto internalState = resource.get_internal_state<BufferVulkan>();

        resource.set_internal_state(nullptr);

        // #TODO: Handle deferred deletion
    }

    void Device::begin_render_pass(Swapchain& swapchain, CommandList& cmdList)
    {
        auto vkDeviceInternalState = GET_VK_DEVICE_STATE();
        auto vkCmdListInternalState = cmdList.get_internal_state<CommandListVulkan>();
        auto cmd = vkCmdListInternalState->cmd;
        auto vkSwapchainInternalState = swapchain.get_internal_state<SwapChainVulkan>();

        auto result = vkDeviceInternalState->device.acquireNextImageKHR(
            vkSwapchainInternalState->swapchain, std::uint64_t(-1), vkSwapchainInternalState->acquireSemaphore, {});
        vkSwapchainInternalState->imageIndex = result.value;

        if (result.result != vk::Result::eSuccess)
        {
            // #TODO: Handle out-of-date swapchain
        }

        vk::ImageMemoryBarrier2 barrier{};
        barrier.setImage(vkSwapchainInternalState->images[vkSwapchainInternalState->imageIndex]);
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
        vkCmdListInternalState->preRenderPassBarriers.push_back(barrier);

        barrier.setOldLayout(vk::ImageLayout::eColorAttachmentOptimal);
        barrier.setNewLayout(vk::ImageLayout::ePresentSrcKHR);
        barrier.setSrcStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput);
        barrier.setDstStageMask(vk::PipelineStageFlagBits2::eBottomOfPipe);
        barrier.setSrcAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite);
        barrier.setDstAccessMask(vk::AccessFlagBits2::eNone);
        vkCmdListInternalState->postRenderPassBarriers.push_back(barrier);

        vk::DependencyInfo depInfo{};
        depInfo.setImageMemoryBarriers(vkCmdListInternalState->preRenderPassBarriers);
        cmd.pipelineBarrier2(depInfo);
        vkCmdListInternalState->preRenderPassBarriers.clear();

        vk::RenderingAttachmentInfo attachment{};
        attachment.setImageView(vkSwapchainInternalState->imageViews[vkSwapchainInternalState->imageIndex]);
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

        vkDeviceInternalState->submitWaitSemaphores.push_back(vkSwapchainInternalState->acquireSemaphore);
        vkDeviceInternalState->submitSignalSemaphores.push_back(vkSwapchainInternalState->releaseSemaphore);
    }

    void Device::end_render_pass(CommandList& cmdList)
    {
        auto vkCmdListInternalState = cmdList.get_internal_state<CommandListVulkan>();
        auto cmd = vkCmdListInternalState->cmd;

        vkCmdListInternalState->cmd.endRendering();

        vk::DependencyInfo depInfo{};
        depInfo.setImageMemoryBarriers(vkCmdListInternalState->postRenderPassBarriers);
        cmd.pipelineBarrier2(depInfo);
        vkCmdListInternalState->postRenderPassBarriers.clear();
    }

    void Device::set_pipeline_state(PipelineState& state, CommandList& cmdList)
    {
        auto vkDeviceInternalState = GET_VK_DEVICE_STATE();
        auto vkCmdListInternalState = cmdList.get_internal_state<CommandListVulkan>();
        auto cmd = vkCmdListInternalState->cmd;

        assert(state.program);
        if (state.program == nullptr)
        {
            // #TODO: Error
            return;
        }
        auto vkProgramInternalState = state.program->get_internal_state<ShaderProgramVulkan>();

        // #TODO: Hash/Get/Create pipeline (base on current render pass)

        cmd.bindPipeline(vkProgramInternalState->bindPoint, {});

        // cmd.setPrimitiveTopology();
    }

}
