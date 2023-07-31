#include "surface_vk.hpp"

#include "device_vk.hpp"
#include "image_vk.hpp"

namespace rune::rhi
{
    SurfaceVulkan::SurfaceVulkan(DeviceVulkan& device, const SurfaceDecl& decl) : m_device(device)
    {
        auto vkInstance = m_device.get_vk_instance();
        auto vkDevice = m_device.get_vk_device();

#ifdef RUNE_PLATFORM_WINDOWS
        vk::Win32SurfaceCreateInfoKHR surfaceInfo{};
        surfaceInfo.setHinstance(static_cast<HINSTANCE>(decl.DisplayHandle));
        surfaceInfo.setHwnd(static_cast<HWND>(decl.SurfaceHandle));
        m_surface = vkInstance.createWin32SurfaceKHR(surfaceInfo);
#endif

        m_imageFence = vkDevice.createFence({});

        resize(decl.Size);
    }

    SurfaceVulkan::~SurfaceVulkan()
    {
        auto vkInstance = m_device.get_vk_instance();
        auto vkDevice = m_device.get_vk_device();

        vkDevice.destroy(m_swapchain);
        vkInstance.destroy(m_surface);
    }

    void SurfaceVulkan::resize(const glm::ivec2& size)
    {
        if (size == m_size)
            return;

        auto vkDevice = m_device.get_vk_device();

        auto oldSwapchain = m_swapchain;

        auto surfaceCaps = m_device.get_vk_physical_device().getSurfaceCapabilitiesKHR(m_surface);
        u32 imageCount = surfaceCaps.minImageCount + 1;
        if (surfaceCaps.maxImageCount > 0 && imageCount > surfaceCaps.maxImageCount)
        {
            imageCount = surfaceCaps.maxImageCount;
        }

        vk::SurfaceFormatKHR surfaceFormat{ vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear };

        vk::PresentModeKHR presentMode = vk::PresentModeKHR::eFifo;

        vk::SwapchainCreateInfoKHR swapchainInfo{};
        swapchainInfo.setSurface(m_surface);
        swapchainInfo.setMinImageCount(imageCount);
        swapchainInfo.setImageFormat(surfaceFormat.format);
        swapchainInfo.setImageColorSpace(surfaceFormat.colorSpace);
        swapchainInfo.setImageExtent({ u32(size.x), u32(size.y) });
        swapchainInfo.setImageArrayLayers(1);
        swapchainInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
        swapchainInfo.setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity);
        swapchainInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
        swapchainInfo.setPresentMode(presentMode);
        swapchainInfo.setClipped(true);
        swapchainInfo.setOldSwapchain(oldSwapchain);
        m_swapchain = vkDevice.createSwapchainKHR(swapchainInfo);

        if (oldSwapchain)
        {
            vkDevice.destroy(oldSwapchain);
        }

        auto imageSize = glm::ivec3(size.x, size.y, 1);
        auto images = vkDevice.getSwapchainImagesKHR(m_swapchain);
        m_images.clear();
        for (auto image : images)
        {
            m_images.push_back(create_owned<ImageVulkan>(m_device, image, surfaceFormat.format, imageSize));
        }

        m_size = size;
    }

    void SurfaceVulkan::acquire_next_image()
    {
        auto vkDevice = m_device.get_vk_device();

        m_imageIndex = vkDevice.acquireNextImageKHR(m_swapchain, u64(-1), {}, m_imageFence).value;
        RUNE_UNUSED(vkDevice.waitForFences(m_imageFence, true, u64(-1)));
        vkDevice.resetFences(m_imageFence);
    }

    void SurfaceVulkan::present()
    {
        auto queue = m_device.get_vk_graphics_queue();

        vk::PresentInfoKHR presentInfo{};
        presentInfo.setImageIndices(m_imageIndex);
        presentInfo.setSwapchains(m_swapchain);
        RUNE_UNUSED(queue.presentKHR(presentInfo));
    }

    auto SurfaceVulkan::current_image() -> Image*
    {
        return m_images[m_imageIndex].get();
    }

}