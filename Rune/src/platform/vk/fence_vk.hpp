#pragma once

#include "rune/rendering/rhi/fence.hpp"

#include <vulkan/vulkan.hpp>

namespace rune::rhi
{
    class DeviceVulkan;

    class FenceVulkan : public Fence
    {
    public:
        FenceVulkan(Shared<DeviceVulkan> device, u64 initialValue);
        ~FenceVulkan();

        auto get_vk_semaphore() const -> vk::Semaphore { return m_semaphore; }

        auto completed_value() const -> u64 override;

        void signal(u64 value) override;
        void wait(u64 value) override;

    private:
        Shared<DeviceVulkan> m_device{ nullptr };
        vk::Semaphore m_semaphore{};
    };
}