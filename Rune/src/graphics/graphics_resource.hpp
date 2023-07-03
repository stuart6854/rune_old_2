#pragma once

#include "graphics/graphics.hpp"
#include "resources/resource.hpp"

namespace rune::graphics
{
    class GraphicsResource : public resources::Resource
    {
    public:
        explicit GraphicsResource(gfx::DeviceHandle device) : m_device(device) {}
        ~GraphicsResource() override = default;

        auto get_device() const -> gfx::DeviceHandle { return m_device; }

    private:
        gfx::DeviceHandle m_device{};
    };
}