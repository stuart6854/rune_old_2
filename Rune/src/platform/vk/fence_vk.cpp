#include "fence_vk.hpp"

#include "device_vk.hpp"

namespace rune::rhi
{
    auto Fence::create(Shared<Device> device, u64 initialValue) -> Owned<Fence>
    {
        return create_owned<FenceVulkan>(static_pointer_cast<DeviceVulkan>(device), initialValue);
    }

    FenceVulkan::FenceVulkan(Shared<DeviceVulkan> device, u64 initialValue) : m_device(device)
    {
        auto vkDevice = m_device->get_vk_device();

        vk::SemaphoreTypeCreateInfo typeInfo{};
        typeInfo.setSemaphoreType(vk::SemaphoreType::eTimeline);
        typeInfo.setInitialValue(initialValue);

        vk::SemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.setPNext(&typeInfo);

        m_semaphore = vkDevice.createSemaphore(semaphoreInfo);
    }

    FenceVulkan::~FenceVulkan()
    {
        auto vkDevice = m_device->get_vk_device();

        vkDevice.destroy(m_semaphore);
    }

    auto FenceVulkan::completed_value() const -> u64
    {
        auto vkDevice = m_device->get_vk_device();

        return vkDevice.getSemaphoreCounterValue(m_semaphore);
    }

    void FenceVulkan::signal(u64 value)
    {
        auto vkDevice = m_device->get_vk_device();

        vk::SemaphoreSignalInfo signalInfo{};
        signalInfo.setSemaphore(m_semaphore);
        signalInfo.setValue(value);
        vkDevice.signalSemaphore(signalInfo);
    }

    void FenceVulkan::wait(u64 value)
    {
        auto vkDevice = m_device->get_vk_device();

        vk::SemaphoreWaitInfo waitInfo{};
        waitInfo.setSemaphores(m_semaphore);
        waitInfo.setValues(value);
        RUNE_UNUSED(vkDevice.waitSemaphores(waitInfo, u64(-1)));
    }

}