#pragma once

#include "enums.hpp"

#include <cstdint>
#include <cstddef>
#include <array>
#include <vector>
#include <string>

namespace rune::rhi
{
    struct SwapChainDesc
    {
        std::uint32_t width = 0;
        std::uint32_t height = 0;
        uint32_t bufferCount = 2;
        Format format = Format::B8G8R8A8Srgb;
        // bool fullscreen = false;
        bool vsync = true;
        float clearColor[4] = { 0, 0, 0, 1 };
        // bool allowHDR = true;
    };

    struct VertexAttribute
    {
        std::uint32_t binding;
        std::uint32_t location;
        Format format;
        std::uint32_t offset;

        VertexAttribute() = default;
        VertexAttribute(std::uint32_t binding, std::uint32_t location, Format format, std::uint32_t offset)
            : binding(binding), location(location), format(format), offset(offset)
        {
        }
    };

    struct VertexBinding
    {
        std::uint32_t binding;
        std::uint32_t stride;
        // InputRate - https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkVertexInputRate.html

        VertexBinding() = default;
        VertexBinding(std::uint32_t binding, std::uint32_t stride) : binding(binding), stride(stride) {}
    };

    struct VertexInputFormat
    {
        std::vector<VertexAttribute> Attributes{};
        std::vector<VertexBinding> Bindings{};
    };

    struct ShaderStageDesc
    {
        std::vector<std::uint8_t> byteCode{};
    };

    struct ShaderProgramDesc
    {
        struct Stages
        {
            ShaderStageDesc compute;
            ShaderStageDesc vertex;
            ShaderStageDesc fragment;
        } stages;

        VertexInputFormat vertexInputFormat;  // Vertex Input should be reflected (or, optionally, defined)?
        // Attachment Targets will be determined during binding (based on current render pass).
    };

    struct BufferDesc
    {
        std::uint64_t size = 0;
        ReadWriteUsage usage = ReadWriteUsage::Default;
        UsageFlags bindFlags = UsageFlags::None;
        // std::uint32_t stride = 0; ??  // For structured buffer types
        // Format format; ??   // For typed buffer
    };

    struct TextureDesc
    {
        Format format;
        std::uint32_t width = 0;
        std::uint32_t height = 0;
        std::uint32_t mipLevels = 1;
        std::uint32_t sampleCount = 1;
    };
}

#define RUNE_RHI_HASH_COMBINE(seed, value) \
    seed ^= std::hash<std::decay<decltype(value)>::type>{}(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2)

namespace std
{
    template <>
    struct hash<std::vector<std::uint8_t>>
    {
        std::size_t operator()(const std::vector<std::uint8_t>& vec) const noexcept
        {
            std::size_t seed = 0;
            RUNE_RHI_HASH_COMBINE(seed, vec.size());
            for (auto& v : vec)
                RUNE_RHI_HASH_COMBINE(seed, v);

            return seed;
        }
    };

    template <>
    struct hash<rune::rhi::ShaderProgramDesc>
    {
        std::size_t operator()(const rune::rhi::ShaderProgramDesc& desc) const noexcept
        {
            std::size_t seed = 0;
            RUNE_RHI_HASH_COMBINE(seed, desc.stages.compute.byteCode);
            RUNE_RHI_HASH_COMBINE(seed, desc.stages.vertex.byteCode);
            RUNE_RHI_HASH_COMBINE(seed, desc.stages.fragment.byteCode);

            return seed;
        }
    };
}