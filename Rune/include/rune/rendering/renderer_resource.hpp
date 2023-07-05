#pragma once

#include "system_renderer.hpp"
#include "resources/resource.hpp"

namespace rune
{
    class RendererResource : public Resource
    {
    public:
        explicit RendererResource(SystemRenderer& renderer) : m_renderer(renderer) {}
        ~RendererResource() override = default;

        auto get_renderer() const -> SystemRenderer& { return m_renderer; }

    private:
        SystemRenderer& m_renderer;
    };
}