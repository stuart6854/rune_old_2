#pragma once

#include <vulkan/vulkan.hpp>

namespace rune::rhi
{
    class InstanceVulkan
    {
    public:
        bool initialise();
        bool destroy();

        auto get_vk_instance() const -> vk::Instance { return m_instance; }

        auto get_best_vk_physical_device() const -> vk::PhysicalDevice;

    private:
        vk::DynamicLoader m_loader{};
        vk::Instance m_instance{};
        vk::DebugUtilsMessengerEXT m_debugMessenger{};
    };

}