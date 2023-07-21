#pragma once

#include "rune/common.hpp"

#include <glm/ext/vector_int2.hpp>
#include <glm/ext/vector_float4.hpp>

namespace rune::rhi
{
    class Surface;

    struct RenderTargetDecl
    {
        Surface* Surface{ nullptr };

        // Begin Offscreen Target (Not required if using Surface)
        glm::ivec2 Size{};

        // End Offscreen Target

        bool ClearColorEnable;
        glm::vec4 ClearColor{};
    };

    class Device;
    class Surface;

    class RenderTarget
    {
    public:
        static auto create(Shared<Device> device, const RenderTargetDecl& decl) -> Owned<RenderTarget>;

        virtual ~RenderTarget() = default;

        virtual void resize(const glm::ivec2& size) = 0;
    };
}