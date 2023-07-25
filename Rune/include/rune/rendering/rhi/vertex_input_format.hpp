#pragma once

#include "rune/common.hpp"
#include "common.hpp"

namespace rune::rhi
{
    struct VertexAttribute
    {
        u32 Binding{};
        u32 Location{};
        Format Format{};
        u32 Offset{};
    };

    struct VertexBinding
    {
        u32 Binding{};
        u32 Stride{};
        // InputRate - https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkVertexInputRate.html
    };

    struct VertexInputFormatDecl
    {
        std::vector<VertexAttribute> Attributes{};
        std::vector<VertexBinding> Bindings{};
    };

    class VertexInputFormat
    {
    public:
        static auto create(const VertexInputFormatDecl& decl) -> Owned<VertexInputFormat>;

        virtual ~VertexInputFormat() = default;
    };
}