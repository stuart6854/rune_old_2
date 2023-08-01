#include "type_conversions_vk.hpp"

#include <cassert>

namespace rune::rhi
{
    auto convert(Format format) -> vk::Format
    {
        switch (format)
        {
            case rune::rhi::Format::Undefined: return vk::Format::eUndefined;
            case rune::rhi::Format::R8G8B8Unorm: return vk::Format::eR8G8B8Unorm;
            case rune::rhi::Format::R8G8B8Srgb: return vk::Format::eR8G8B8Srgb;
            case rune::rhi::Format::R8G8B8A8Unorm: return vk::Format::eR8G8B8A8Unorm;
            case rune::rhi::Format::RGB32Sfloat: return vk::Format::eR32G32B32Sfloat;
            case rune::rhi::Format::RGBA32Sfloat: return vk::Format::eR32G32B32A32Sfloat;
            case rune::rhi::Format::R8G8B8A8Srgb: return vk::Format::eR8G8B8A8Srgb;
            case rune::rhi::Format::B8G8R8A8Srgb: return vk::Format::eB8G8R8A8Srgb;
            case rune::rhi::Format::Depth24Stencil8: return vk::Format::eD24UnormS8Uint;
            case rune::rhi::Format::Depth32: return vk::Format::eD32Sfloat;
        }
        assert(false);
        return {};
    }

    auto convert(ShaderStages stage) -> vk::ShaderStageFlagBits
    {
        switch (stage)
        {
            case rune::rhi::ShaderStages::Vertex: return vk::ShaderStageFlagBits::eVertex;
            case rune::rhi::ShaderStages::Fragment: return vk::ShaderStageFlagBits::eFragment;
            case rune::rhi::ShaderStages::Compute: return vk::ShaderStageFlagBits::eCompute;
        }
        assert(false);
        return {};
    }

    auto convert(Topology topology) -> vk::PrimitiveTopology
    {
        switch (topology)
        {
            case rune::rhi::Topology::PointList: return vk::PrimitiveTopology::ePointList;
            case rune::rhi::Topology::LineList: return vk::PrimitiveTopology::eLineList;
            case rune::rhi::Topology::TriangleList: return vk::PrimitiveTopology::eTriangleList;
        }
        assert(false);
        return {};
    }

    auto convert(CullMode cullMode) -> vk::CullModeFlags
    {
        switch (cullMode)
        {
            case rune::rhi::CullMode::None: return vk::CullModeFlagBits::eNone;
            case rune::rhi::CullMode::Front: return vk::CullModeFlagBits::eFront;
            case rune::rhi::CullMode::Back: return vk::CullModeFlagBits::eBack;
        }
        assert(false);
        return {};
    }

}