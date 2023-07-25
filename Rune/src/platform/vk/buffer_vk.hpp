#pragma once

#include "rune/rendering/rhi/buffer.hpp"

#include <vulkan/vulkan.hpp>
#include <vulkan-memory-allocator-hpp/vk_mem_alloc.hpp>

namespace rune::rhi
{
    class DeviceVulkan;

    class BufferVulkan : public Buffer
    {
    public:
        BufferVulkan(Shared<DeviceVulkan> device, const BufferDecl& decl);
        ~BufferVulkan();

        auto get_vk_buffer() const -> vk::Buffer { return m_buffer; }

        auto type() const -> BufferType override { return m_decl.Type; }
        auto size() const -> u64 override { return m_decl.Size; }

        void write(u64 offset, u64 size, const void* data) override;

    private:
        Shared<DeviceVulkan> m_device{ nullptr };

        BufferDecl m_decl{};
        vk::Buffer m_buffer{};
        vma::Allocation m_allocation{};
    };
}