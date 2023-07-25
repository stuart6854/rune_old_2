#include "type_conversions_vk.hpp"

#include "rune/debug/log_engine.hpp"
#include "rune/debug/assert_engine.hpp"

namespace rune::rhi
{
    auto convert(Format format) -> vk::Format
    {
        switch (format)
        {
            case Format::R8: return vk::Format::eR8Unorm;
            case Format::R32: return vk::Format::eR32Sfloat;
            case Format::RG32: return vk::Format::eR32G32Sfloat;
            case Format::RGB32: return vk::Format::eR32G32B32Sfloat;
            case Format::RGBA8: return vk::Format::eR8G8B8A8Srgb;
            case Format::RGBA32: return vk::Format::eR32G32B32A32Sfloat;
            case Format::BGRA8: return vk::Format::eB8G8R8A8Srgb;
        }

        RUNE_ENG_ERROR("Unknown Format.");
        RUNE_ENG_ASSERT(false);
        return {};
    }

    auto convert(vk::Format format) -> Format
    {
        switch (format)
        {
            case vk::Format::eR8Unorm: return Format::R8;
            case vk::Format::eR32Sfloat: return Format::R32;
            case vk::Format::eR32G32Sfloat: return Format::RG32;
            case vk::Format::eR32G32B32Sfloat: return Format::RGB32;
            case vk::Format::eR8G8B8A8Srgb: return Format::RGBA8;
            case vk::Format::eR32G32B32A32Sfloat: return Format::RGBA32;
            case vk::Format::eB8G8R8A8Srgb: return Format::BGRA8;
        }

        RUNE_ENG_ERROR("Unknown vk::Format.");
        RUNE_ENG_ASSERT(false);
        return {};
    }

    auto convert(BufferType type) -> vk::BufferUsageFlags
    {
        switch (type)
        {
            case BufferType::Vertex: return vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst;
            case BufferType::Index: return vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst;
            case BufferType::Uniform: return vk::BufferUsageFlagBits::eUniformBuffer;
            case BufferType::Storage: return vk::BufferUsageFlagBits::eStorageBuffer;
            case BufferType::Transfer: return vk::BufferUsageFlagBits::eTransferSrc;
        }

        RUNE_ENG_ERROR("Unknown BufferType.");
        RUNE_ENG_ASSERT(false);
        return {};
    }

    auto convert(ImageUsage usage) -> vk::ImageUsageFlags
    {
        switch (usage)
        {
            case ImageUsage::Texture: return vk::ImageUsageFlagBits::eSampled;
            case ImageUsage::AttachmentColor: return vk::ImageUsageFlagBits::eColorAttachment;
            case ImageUsage::AttachmentDepthStencil: return vk::ImageUsageFlagBits::eDepthStencilAttachment;
        }

        RUNE_ENG_ERROR("Unknown ImageUsage.");
        RUNE_ENG_ASSERT(false);
        return {};
    }

    auto convert(vk::ImageUsageFlagBits usage) -> ImageUsage
    {
        switch (usage)
        {
            case vk::ImageUsageFlagBits::eSampled: return ImageUsage::Texture;
            case vk::ImageUsageFlagBits::eColorAttachment: return ImageUsage::AttachmentColor;
            case vk::ImageUsageFlagBits::eDepthStencilAttachment: return ImageUsage::AttachmentDepthStencil;
        }

        RUNE_ENG_ERROR("Unknown vk::ImageUsageFlag.");
        RUNE_ENG_ASSERT(false);
        return {};
    }

    auto convert(ShaderType type) -> vk::ShaderStageFlagBits
    {
        switch (type)
        {
            case rune::rhi::ShaderType::Vertex: return vk::ShaderStageFlagBits::eVertex;
            case rune::rhi::ShaderType::Hull: return vk::ShaderStageFlagBits::eTessellationControl;
            case rune::rhi::ShaderType::Domain: return vk::ShaderStageFlagBits::eTessellationEvaluation;
            case rune::rhi::ShaderType::Geometry: return vk::ShaderStageFlagBits::eGeometry;
            case rune::rhi::ShaderType::Pixel: return vk::ShaderStageFlagBits::eFragment;
            case rune::rhi::ShaderType::Compute: return vk::ShaderStageFlagBits::eCompute;
        }

        RUNE_ENG_ERROR("Unknown ShaderType.");
        RUNE_ENG_ASSERT(false);
        return {};
    }

    auto convert_to_layout(ResourceState state) -> vk::ImageLayout
    {
        switch (state)
        {
            case ResourceState::Undefined: return vk::ImageLayout::eUndefined;
            case ResourceState::ShaderRead: return vk::ImageLayout::eShaderReadOnlyOptimal;
            case ResourceState::RenderTarget: return vk::ImageLayout::eColorAttachmentOptimal;
            case ResourceState::Present: return vk::ImageLayout::ePresentSrcKHR;
        }

        RUNE_ENG_ERROR("Unknown ResourceState.");
        RUNE_ENG_ASSERT(false);
        return {};
    }

    auto convert_to_stage(ResourceState state) -> vk::PipelineStageFlags2
    {
        switch (state)
        {
            case rune::rhi::ResourceState::Undefined: return vk::PipelineStageFlagBits2::eTopOfPipe;
            case rune::rhi::ResourceState::ShaderRead: return vk::PipelineStageFlagBits2::eFragmentShader;
            case rune::rhi::ResourceState::RenderTarget: return vk::PipelineStageFlagBits2::eColorAttachmentOutput;
            case rune::rhi::ResourceState::Present: return vk::PipelineStageFlagBits2::eBottomOfPipe;
        }

        RUNE_ENG_ERROR("Unknown ResourceState.");
        RUNE_ENG_ASSERT(false);
        return {};
    }

    auto convert_to_access(ResourceState state) -> vk::AccessFlags2
    {
        switch (state)
        {
            case rune::rhi::ResourceState::Undefined: return vk::AccessFlagBits2::eNone;
            case rune::rhi::ResourceState::ShaderRead: return vk::AccessFlagBits2::eShaderRead;
            case rune::rhi::ResourceState::RenderTarget: return vk::AccessFlagBits2::eColorAttachmentWrite;
            case rune::rhi::ResourceState::Present: return vk::AccessFlagBits2::eNone;
        }

        RUNE_ENG_ERROR("Unknown ResourceState.");
        RUNE_ENG_ASSERT(false);
        return {};
    }

    auto convert(PrimitiveTopology topology) -> vk::PrimitiveTopology
    {
        switch (topology)
        {
            case rune::rhi::PrimitiveTopology::LineList: return vk::PrimitiveTopology::eLineList;
            case rune::rhi::PrimitiveTopology::TriangleList: return vk::PrimitiveTopology::eTriangleList;
        }

        RUNE_ENG_ERROR("Unknown Topology.");
        RUNE_ENG_ASSERT(false);
        return {};
    }

    auto convert(CullMode cullMode) -> vk::CullModeFlags
    {
        switch (cullMode)
        {
            case rune::rhi::CullMode::None: return vk::CullModeFlagBits::eNone;
            case rune::rhi::CullMode::Front: return vk::CullModeFlagBits::eFront;
            case rune::rhi::CullMode::Back: return vk::CullModeFlagBits::eBack;
            case rune::rhi::CullMode::FrontAndBack: return vk::CullModeFlagBits::eFrontAndBack;
        }

        RUNE_ENG_ERROR("Unknown CullMode.");
        RUNE_ENG_ASSERT(false);
        return {};
    }

    auto convert(IndexType indexType) -> vk::IndexType
    {
        switch (indexType)
        {
            case rune::rhi::IndexType::U16: return vk::IndexType::eUint16;
            case rune::rhi::IndexType::U32: return vk::IndexType::eUint32;
        }

        RUNE_ENG_ERROR("Unknown IndexType.");
        RUNE_ENG_ASSERT(false);
        return {};
    }

}