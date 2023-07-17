#pragma once

#include "common.hpp"
#include "core/system.hpp"
#include "events/system_events.hpp"

#include <gfx/gfx.hpp>

namespace rune
{
    using namespace sm;

    class SystemImGui : public ISystem
    {
    public:
        void initialise() override;
        void update() override;
        void shutdown() override;

    private:
        static void receive_event(const Event& event);

        void render_ui(gfx::CommandListHandle cmdList, u32 frameIndex);

    private:
        gfx::PipelineHandle m_pipeline{};
        struct RenderBuffer
        {
            gfx::BufferHandle vtxBuffer{};
            sizet vertexBufferSize{};
            gfx::BufferHandle idxBuffer{};
            sizet indexBufferSize{};
        };
        std::array<RenderBuffer, 2> m_renderBuffers{};

        gfx::TextureHandle m_fontTexture{};
        gfx::DescriptorSetHandle m_fontTextureSet{};
    };
}