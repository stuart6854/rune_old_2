#pragma once

#include "rune/rendering/rhi/device.hpp"

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.hpp>

namespace rune::rhi
{
    class InstanceVulkan;
    class Fence;

    class DeviceVulkan : public Device
    {
    public:
        bool initialise();
        bool destroy();

        auto get_vk_instance() const -> vk::Instance;
        auto get_vk_physical_device() const -> vk::PhysicalDevice { return m_physicalDevice; }
        auto get_vk_device() const -> vk::Device { return m_device; }
        auto get_vk_allocator() const -> vma::Allocator { return m_allocator; }
        auto get_vk_graphics_queue() const -> vk::Queue { return m_graphicsQueue; }
        auto get_vk_cmd_pool() const -> vk::CommandPool { return m_cmdPool; }

        void submit(const std::vector<CommandList*>& cmdLists, Fence* fence, u64 signalValue) override;

        // void mark_for_destruction(Owned<Object>&& object);

    private:
        Shared<InstanceVulkan> m_instance{};
        vk::PhysicalDevice m_physicalDevice{};
        vk::Device m_device{};
        vma::Allocator m_allocator{};

        vk::Queue m_graphicsQueue{};

        vk::CommandPool m_cmdPool{};
    };
}