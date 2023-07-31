#pragma once

#include "rune/common.hpp"
#include "rune/rendering/rhi/surface.hpp"
#include "image_vk.hpp"

#include <vulkan/vulkan.hpp>

#include <glm/ext/vector_int2.hpp>

namespace rune::rhi
{
    class DeviceVulkan;
    class ImageVulkan;

    class SurfaceVulkan : public Surface
    {
    public:
        SurfaceVulkan(DeviceVulkan& device, const SurfaceDecl& decl);
        ~SurfaceVulkan();

        auto get_vk_swapchain() const -> auto { return m_swapchain; }
        auto get_vk_image_index() const -> auto { return m_imageIndex; }

        auto size() const -> const glm::ivec2& { return m_size; }

        void resize(const glm::ivec2& size);

        void acquire_next_image() override;
        void present() override;

        auto current_image() -> Image* override;

    private:
        DeviceVulkan& m_device;

        vk::SurfaceKHR m_surface{};
        vk::SwapchainKHR m_swapchain{};
        glm::ivec2 m_size{};

        std::vector<Owned<ImageVulkan>> m_images{};
        u32 m_imageIndex{};

        vk::Fence m_imageFence{};
    };
}