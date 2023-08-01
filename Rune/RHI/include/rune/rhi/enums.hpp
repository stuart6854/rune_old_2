#pragma once

#include "utils/flags.hpp"

namespace rune::rhi
{
    enum class QueueType
    {
        Graphics,
        Compute,
        Copy,
    };

    enum class ResourceState
    {
        Undefined,
        RenderTarget,
        DepthStencil,
        ShaderResource,
    };

    enum class Format
    {
        Undefined,
        R8G8B8Unorm,
        R8G8B8Srgb,
        R8G8B8A8Unorm,
        R8G8B8A8Srgb,

        B8G8R8A8Srgb,

        Depth24Stencil8,
        Depth32,
    };

    enum class ReadWriteUsage
    {
        Default,   // CPU=No access, GPU=Read/Write
        Upload,    // CPU=Write, GPU=Read
        ReadBack,  // CPU=Read, GPU=Write
    };

    enum class UsageFlags
    {
        None = 0,
        VertexBuffer = 1 << 0,
        IndexBuffer = 1 << 1,
        UniformBuffer = 1 << 2,
        StorageBuffer = 1 << 3,
        ShaderResource = 1 << 4,
        RenderTarget = 1 << 5,
    };
    DEFINE_ENUM_CLASS_BITWISE_OPERATORS(UsageFlags);

    enum class ShaderStages : std::uint8_t
    {
        Vertex,
        TessellationControl,
        TessellationEvaluation,
        Geometry,
        Fragment,
        Compute,
        Count,
    };

    enum class Topology
    {
        PointList,
        LineList,
        TriangleList,
    };

    enum class FillMode
    {
        Point,
        Line,
        Fill,
    };

    enum class CullMode
    {
        None,
        Front,
        Back,
    };
}