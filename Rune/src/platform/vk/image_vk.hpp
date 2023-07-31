#pragma once

#include "rune/rendering/rhi/image.hpp"

#include <vulkan/vulkan.hpp>
#include <vulkan-memory-allocator-hpp/vk_mem_alloc.hpp>

namespace rune::rhi
{
    class DeviceVulkan;

    class ImageVulkan : public Image
    {
    public:
        ImageVulkan(DeviceVulkan& device, const ImageDecl& decl);
        ImageVulkan(DeviceVulkan& device, vk::Image image, vk::Format format, const glm::ivec3& size);
        ~ImageVulkan();

        auto get_vk_image() const -> vk::Image { return m_image; }
        auto get_vk_view() const -> vk::ImageView { return m_view; }
        auto get_vk_subresource_range() const -> vk::ImageSubresourceRange { return m_subresourceRange; }

        auto size() const -> glm::ivec3 override { return m_decl.Size; }
        auto format() const -> Format override { return m_decl.Format; }
        auto usage() const -> ImageUsage override { return m_decl.Usage; }

    private:
        void create_view();

    private:
        DeviceVulkan& m_device;

        vk::Image m_image{};
        vma::Allocation m_allocation{};

        ImageDecl m_decl{};

        vk::ImageView m_view{};
        vk::ImageSubresourceRange m_subresourceRange{};
    };

}