#include "buffer_vk.hpp"

#include "device_vk.hpp"
#include "type_conversions_vk.hpp"

namespace rune::rhi
{
    auto Buffer::create(Shared<Device> device, const BufferDecl& decl) -> Owned<Buffer>
    {
        return create_owned<BufferVulkan>(static_pointer_cast<DeviceVulkan>(device), decl);
    }

    BufferVulkan::BufferVulkan(Shared<DeviceVulkan> device, const BufferDecl& decl) : m_device(device), m_decl(decl)
    {
        vk::BufferCreateInfo bufferInfo{};
        bufferInfo.setSize(decl.Size);
        bufferInfo.setUsage(convert(decl.Type));

        vma::AllocationCreateInfo allocationInfo{};
        allocationInfo.setUsage(vma::MemoryUsage::eAuto);
        if (m_decl.AllowCPUAccess)
            allocationInfo.setFlags(vma::AllocationCreateFlagBits::eHostAccessSequentialWrite);

        auto allocator = m_device->get_vk_allocator();
        std::tie(m_buffer, m_allocation) = allocator.createBuffer(bufferInfo, allocationInfo, nullptr);
    }

    BufferVulkan::~BufferVulkan()
    {
        if (m_device)
        {
            auto allocator = m_device->get_vk_allocator();
            allocator.destroyBuffer(m_buffer, m_allocation);
        }
    }

    void BufferVulkan::write(u64 offset, u64 size, const void* data)
    {
        auto vkAllocator = m_device->get_vk_allocator();

        auto* mappedPtr = vkAllocator.mapMemory(m_allocation);
        u8* offsetMappedPtr = static_cast<u8*>(mappedPtr) + offset;
        std::memcpy(offsetMappedPtr, data, size);
        vkAllocator.unmapMemory(m_allocation);
    }

}