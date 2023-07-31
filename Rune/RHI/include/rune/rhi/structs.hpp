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
        bool enabled = false;
        std::string sourceFilename;
        std::string sourceCode;
        std::string sourceEntryPoint;

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