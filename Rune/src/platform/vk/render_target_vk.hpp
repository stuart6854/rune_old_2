#pragma once

#include "rune/rendering/rhi/render_target.hpp"

#include <vulkan/vulkan.hpp>

namespace rune::rhi
{
    class DeviceVulkan;
    class SurfaceVulkan;

    class RenderTargetVulkan : public RenderTarget
    {
    public:
        RenderTargetVulkan(Shared<DeviceVulkan> device, const RenderTargetDecl& decl);
        ~RenderTargetVulkan() = default;

        auto get_vk_rendering_info() -> const vk::RenderingInfo&;

        void resize(const glm::ivec2& size) override;

    private:
        SurfaceVulkan* m_surface{ nullptr };

        std::vector<vk::RenderingAttachmentInfo> m_colorAttachments{};
        vk::RenderingInfo m_renderingInfo{};
    };
}