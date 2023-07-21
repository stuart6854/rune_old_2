#include "render_target_vk.hpp"

#include "device_vk.hpp"
#include "surface_vk.hpp"

namespace rune::rhi
{
    auto RenderTarget::create(Shared<Device> device, const RenderTargetDecl& decl) -> Owned<RenderTarget>
    {
        return create_owned<RenderTargetVulkan>(static_pointer_cast<DeviceVulkan>(device), decl);
    }

    RenderTargetVulkan::RenderTargetVulkan(Shared<DeviceVulkan> device, const RenderTargetDecl& decl)
    {
        m_renderingInfo.setRenderArea(vk::Rect2D({ 0, 0 }, { u32(decl.Size.x), u32(decl.Size.y) }));
        m_renderingInfo.setLayerCount(1);

        if (decl.Surface)
        {
            auto clearColorValue = vk::ClearColorValue({ decl.ClearColor.x, decl.ClearColor.y, decl.ClearColor.z, decl.ClearColor.w });

            auto& colorAttachment = m_colorAttachments.emplace_back();
            colorAttachment.setImageLayout(vk::ImageLayout::eColorAttachmentOptimal);
            colorAttachment.setLoadOp(decl.ClearColorEnable ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eLoad);
            colorAttachment.setStoreOp(vk::AttachmentStoreOp::eStore);
            colorAttachment.setClearValue(clearColorValue);

            m_surface = static_cast<SurfaceVulkan*>(decl.Surface);
        }
        else
        {
        }

        m_renderingInfo.setColorAttachments(m_colorAttachments);
    }

    auto RenderTargetVulkan::get_vk_rendering_info() -> const vk::RenderingInfo&
    {
        const auto& size = m_surface->size();

        m_renderingInfo.setRenderArea(vk::Rect2D({ 0, 0 }, { u32(size.x), u32(size.y) }));
        m_renderingInfo.setLayerCount(1);

        auto* image = static_cast<ImageVulkan*>(m_surface->current_image());
        m_colorAttachments[0].setImageView(image->get_vk_view());

        return m_renderingInfo;
    }

    void RenderTargetVulkan::resize(const glm::ivec2& size)
    {
        if (m_surface)
            return;

        RUNE_UNUSED(size);
    }

}