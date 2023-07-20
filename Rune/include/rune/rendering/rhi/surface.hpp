#pragma once

#include <glm/ext/vector_int2.hpp>

namespace rune::rhi
{
    class Device;
    class Image;

    struct SurfaceDecl
    {
        void* DisplayHandle{};  // Win32=HINSTANCE, Wayland=wl_display
        void* SurfaceHandle{};  // Win32=HWND, Wayland=wl_surface
        glm::ivec2 Size{};
    };

    class Surface
    {
    public:
        static auto create(Shared<Device> device, const SurfaceDecl& decl) -> Owned<Surface>;

        virtual ~Surface() = default;

        virtual void acquire_next_image() = 0;
        virtual void present() = 0;

        virtual auto current_image() -> Image* = 0;
    };
}