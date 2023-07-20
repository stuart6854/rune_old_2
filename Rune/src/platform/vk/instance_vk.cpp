#include "instance_vk.hpp"

#include "rune/debug/log_engine.hpp"
#include "rune/debug/assert_engine.hpp"

namespace rune::rhi
{
    VKAPI_ATTR VkBool32 VKAPI_CALL debug_utils_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                                  VkDebugUtilsMessageTypeFlagsEXT /*message_type*/,
                                                                  const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                                                                  void* /*user_data*/)
    {
        if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            RUNE_ENG_WARN("{} - {}: {}", callback_data->messageIdNumber, callback_data->pMessageIdName, callback_data->pMessage);
        }
        else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        {
            RUNE_ENG_ERROR("{} - {}: {}", callback_data->messageIdNumber, callback_data->pMessageIdName, callback_data->pMessage);
            RUNE_ENG_ASSERT(false);
        }
        return VK_FALSE;
    }

    bool InstanceVulkan::initialise()
    {
        PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = m_loader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
        VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

        vk::ApplicationInfo appInfo{};
        appInfo.setPEngineName("RuneEngine");
        appInfo.setEngineVersion(VK_MAKE_API_VERSION(0, 0, 1, 0));
        appInfo.setApiVersion(VK_API_VERSION_1_3);

        std::vector<const char*> extensions{
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,

            VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef RUNE_PLATFORM_WINDOWS
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

        m_instance = vk::createInstance(instanceInfo);
        if (!m_instance)
        {
            RUNE_ENG_ERROR("Failed to create Vulkan instance.");
            destroy();
            return false;
        }
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_instance);

        m_debugMessenger = m_instance.createDebugUtilsMessengerEXT(debugMessengerInfo);
        if (!m_debugMessenger)
        {
            RUNE_ENG_ERROR("Failed to create Vulkan debug messenger.");
        }

        return true;
    }

    bool InstanceVulkan::destroy()
    {
        m_instance.destroy(m_debugMessenger);
        m_debugMessenger = nullptr;

        m_instance.destroy();
        m_instance = nullptr;
        return true;
    }

    auto InstanceVulkan::get_best_vk_physical_device() const -> vk::PhysicalDevice
    {
        auto physicalDevices = m_instance.enumeratePhysicalDevices();
        if (physicalDevices.empty())
            return {};

        if (physicalDevices.size() == 1)
            return physicalDevices[0];

        u32 bestScore = 0;
        auto bestPhysicalDevice = physicalDevices[0];
        for (auto& physicalDevice : physicalDevices)
        {
            u32 score = 0;

            auto props = physicalDevice.getProperties();

            score += props.deviceType == vk::PhysicalDeviceType::eDiscreteGpu ? 1000 : 0;
            score += props.limits.maxImageDimension2D;
            score += props.limits.maxBoundDescriptorSets;

            // #TODO: vkGetPhysicalDeviceMemoryProperties is probably more indicative of performance.
            // As there is usually a strong correlation between local memory size and performance.

            if (score > bestScore)
            {
                bestScore = score;
                bestPhysicalDevice = physicalDevice;
            }
        }

        return bestPhysicalDevice;
    }

}