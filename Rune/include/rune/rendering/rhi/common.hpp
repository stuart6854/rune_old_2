#pragma once

namespace rune::rhi
{
    enum class Format
    {
        R8,
        R32,
        RG32,
        RGB32,
        RGBA8,
        RGBA32,
        BGRA8,
        Depth24,
        Depth32,
    };

    enum class ResourceState
    {
        Undefined,
        ShaderRead,
        RenderTarget,
        Present,
    };

    inline bool is_depth_format(Format format)
    {
        return format >= Format::Depth24;
    }

    inline bool is_color_format(Format format)
    {
        return !is_depth_format(format);
    }
}