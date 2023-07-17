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

    private:
        gfx::TextureHandle m_fontTexture{};
        gfx::DescriptorSetHandle m_fontTextureSet{};
    };
}