#include "image_vk.hpp"

#include "device_vk.hpp"
#include "type_conversions_vk.hpp"

namespace rune::rhi
{
    ImageVulkan::ImageVulkan(Shared<DeviceVulkan> device, const ImageDecl& decl) : m_device(device), m_decl(decl)
    {
        auto vkAllocator = m_device->get_vk_allocator();

        vk::ImageCreateInfo imageInfo{};
        imageInfo.setImageType(decl.Size.z > 1 ? vk::ImageType::e3D : decl.Size.y > 1 ? vk::ImageType::e2D : vk::ImageType::e1D);
        imageInfo.setFormat(convert(decl.Format));
        imageInfo.setExtent({ u32(decl.Size.x), u32(decl.Size.y), u32(decl.Size.z) });
        imageInfo.setMipLevels(1);
        imageInfo.setArrayLayers(1);
        imageInfo.setUsage(convert(decl.Usage));

        vma::AllocationCreateInfo allocInfo{};
        allocInfo.setUsage(vma::MemoryUsage::eAuto);

        std::tie(m_image, m_allocation) = vkAllocator.createImage(imageInfo, allocInfo);

        m_subresourceRange.setAspectMask(is_depth_format(m_decl.Format) ? vk::ImageAspectFlagBits::eDepth
                                                                        : vk::ImageAspectFlagBits::eColor);
        m_subresourceRange.setBaseArrayLayer(0);
        m_subresourceRange.setBaseMipLevel(0);
        m_subresourceRange.setLayerCount(1);
        m_subresourceRange.setLevelCount(1);

        create_view();
    }

    ImageVulkan::ImageVulkan(Shared<DeviceVulkan> device, vk::Image image, vk::Format format, const glm::ivec3& size)
        : m_device(device), m_image(image)
    {
        m_decl.Format = convert(format);
        m_decl.Size = size;

        m_subresourceRange.setAspectMask(is_depth_format(m_decl.Format) ? vk::ImageAspectFlagBits::eDepth
                                                                        : vk::ImageAspectFlagBits::eColor);
        m_subresourceRange.setBaseArrayLayer(0);
        m_subresourceRange.setBaseMipLevel(0);
        m_subresourceRange.setLayerCount(1);
        m_subresourceRange.setLevelCount(1);

        create_view();
    }

    ImageVulkan::~ImageVulkan()
    {
        auto vkDevice = m_device->get_vk_device();
        auto vkAllocator = m_device->get_vk_allocator();

        vkDevice.destroy(m_view);

        if (m_allocation)
        {
            vkAllocator.destroyImage(m_image, m_allocation);
        }
    }

    void ImageVulkan::create_view()
    {
        auto vkDevice = m_device->get_vk_device();

        vk::ImageViewCreateInfo viewInfo{};
        viewInfo.setImage(m_image);
        viewInfo.setFormat(convert(m_decl.Format));
        viewInfo.setViewType(m_decl.Size.z > 1   ? vk::ImageViewType::e3D
                             : m_decl.Size.y > 1 ? vk::ImageViewType::e2D
                                                 : vk::ImageViewType::e1D);
        viewInfo.setSubresourceRange(get_vk_subresource_range());

        m_view = vkDevice.createImageView(viewInfo);
    }

}