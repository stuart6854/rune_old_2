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

        // Vertex Input should be reflected (or, optionally, defined)?
        // Attachment Targets will be determined during binding (based on current render pass).

        // Could also allow (optional) default settings (for early PSO compilation), such as render targets, etc.
    };

    struct BufferDesc
    {
        std::uint64_t size = 0;
        ReadWriteUsage usage = ReadWriteUsage::Default;
        UsageFlags bindFlags = UsageFlags::None;
        // u32 stride = 0; ??  // For structured buffer types
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