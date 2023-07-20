#pragma once

#include "rune/common.hpp"
#include "common.hpp"

#include <glm/ext/vector_int3.hpp>

namespace rune::rhi
{
    enum class ImageUsage
    {
        Texture,
        AttachmentColor,
        AttachmentDepthStencil,
    };

    struct ImageDecl
    {
        glm::ivec3 Size{};
        Format Format{};
        ImageUsage Usage{};
    };

    class Device;

    class Image
    {
    public:
        static auto create(Shared<Device> device, const ImageDecl& decl) -> Owned<Image>;

        virtual ~Image() = default;

        virtual auto size() const -> glm::ivec3 = 0;
        virtual auto format() const -> Format = 0;
        virtual auto usage() const -> ImageUsage = 0;
    };
}