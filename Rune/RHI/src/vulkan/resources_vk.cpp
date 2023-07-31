#include "resources_vk.hpp"

#include "type_conversions_vk.hpp"

namespace rune::rhi
{
    void SwapchainInternal::resize(const SwapChainDesc& desc)
    {
        auto oldSwapchain = swapchain;

        auto surfaceCaps = device->physicalDevice.getSurfaceCapabilitiesKHR(surface);
        std::uint32_t imageCount = desc.bufferCount;
        if (surfaceCaps.maxImageCount > 0 && imageCount > surfaceCaps.maxImageCount)
        {
            imageCount = surfaceCaps.maxImageCount;
        }

        vk::SurfaceFormatKHR surfaceFormat{ vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear };

        vk::PresentModeKHR presentMode = vk::PresentModeKHR::eFifo;

        vk::SwapchainCreateInfoKHR swapchainInfo{};
        swapchainInfo.setSurface(surface);
        swapchainInfo.setMinImageCount(imageCount);
        swapchainInfo.setImageFormat(surfaceFormat.format);
        swapchainInfo.setImageColorSpace(surfaceFormat.colorSpace);
        swapchainInfo.setImageExtent({ desc.width, desc.height });
        swapchainInfo.setImageArrayLayers(1);
        swapchainInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
        swapchainInfo.setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity);
        swapchainInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
        swapchainInfo.setPresentMode(presentMode);
        swapchainInfo.setClipped(true);
        swapchainInfo.setOldSwapchain(oldSwapchain);
        swapchain = device->device.createSwapchainKHR(swapchainInfo);

        if (oldSwapchain)
        {
            device->device.destroy(oldSwapchain);
        }

        images = device->device.getSwapchainImagesKHR(swapchain);
        imageViews.resize(images.size());
        for (auto i = 0; i < images.size(); ++i)
        {
            vk::ImageViewCreateInfo viewInfo{};
            viewInfo.setImage(images[i]);
            viewInfo.setFormat(convert(desc.format));
            viewInfo.setViewType(vk::ImageViewType::e2D);
            viewInfo.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
            viewInfo.subresourceRange.setLayerCount(1);
            viewInfo.subresourceRange.setBaseArrayLayer(0);
            viewInfo.subresourceRange.setLevelCount(1);
            viewInfo.subresourceRange.setBaseMipLevel(0);

            imageViews[i] = device->device.createImageView(viewInfo);
        }

        if (!acquireSemaphore)
            acquireSemaphore = device->device.createSemaphore({});
        if (!releaseSemaphore)
            releaseSemaphore = device->device.createSemaphore({});
    }

}