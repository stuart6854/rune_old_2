#pragma once

#include "structs.hpp"

#include <array>
#include <memory>

namespace rune::rhi
{
    template <typename T>
    struct Resource
    {
        std::shared_ptr<T> internal;

        virtual ~Resource() = default;
        bool is_valid() const { return internal != nullptr; }
    };

    struct CommandListInternal;
    struct CommandList : public Resource<CommandListInternal>
    {
        QueueType queueType;
    };

    struct SwapchainInternal;
    struct Swapchain : public Resource<SwapchainInternal>
    {
        SwapChainDesc desc;  // Visible to App/Engine. Stored here for convenience. (Not necessarily what GPU used for resource)
    };

    struct ShaderProgramInternal;
    struct ShaderProgram : Resource<ShaderProgramInternal>
    {
        ShaderProgramDesc desc;
    };

    template <typename T>
    struct GPUResource : public Resource<T>
    {
        void* mappedData{ nullptr };
        std::size_t mappedSize{ 0 };
    };

    struct BufferInternal;
    struct Buffer : public GPUResource<BufferInternal>
    {
        BufferDesc desc;  // Visible to App/Engine. Stored here for convenience. (Not necessarily what GPU used for resource)
    };

    struct TextureInternal;
    struct Texture : public GPUResource<TextureInternal>
    {
        TextureDesc desc;  // Visible to App/Engine. Stored here for convenience. (Not necessarily what GPU used for resource)
    };

    struct RenderPassImage
    {
        enum class Type
        {
            RenderTarget,
            DepthStencil,
        } type = Type::RenderTarget;
        enum class LoadOp
        {
            Load,
            Clear,
            DontCare,
        } loadOp = LoadOp::Load;
        const Texture* texture = nullptr;
        // int subresource = -1;
        enum class StoreOp
        {
            Store,
            DontCare
        } storeOp = StoreOp::Store;
        ResourceState layoutBefore = ResourceState::Undefined;  // Layout before render pass
        ResourceState layout = ResourceState::Undefined;        // Layout during render pass
        ResourceState layoutAfter = ResourceState::Undefined;   // Layout after render pass

        static auto RenderTarget(const Texture* resource,
                                 LoadOp loadOp = LoadOp::Load,
                                 StoreOp storeOp = StoreOp::Store,
                                 ResourceState layoutBefore = ResourceState::ShaderResource,
                                 ResourceState layoutAfter = ResourceState::ShaderResource) -> RenderPassImage
        {
            RenderPassImage image;
            image.type = Type::RenderTarget;
            image.texture = resource;
            image.loadOp = loadOp;
            image.storeOp = storeOp;
            image.layoutBefore = layoutBefore;
            image.layout = ResourceState::RenderTarget;
            image.layoutAfter = layoutAfter;
            return image;
        }

        static auto DepthStencil(const Texture* resource,
                                 LoadOp loadOp = LoadOp::Load,
                                 StoreOp storeOp = StoreOp::Store,
                                 ResourceState layoutBefore = ResourceState::DepthStencil,
                                 ResourceState layout = ResourceState::DepthStencil,
                                 ResourceState layoutAfter = ResourceState::DepthStencil) -> RenderPassImage
        {
            RenderPassImage image;
            image.type = Type::DepthStencil;
            image.texture = resource;
            image.loadOp = loadOp;
            image.storeOp = storeOp;
            image.layoutBefore = layoutBefore;
            image.layout = layout;
            image.layoutAfter = layoutAfter;
            return image;
        }
    };

    struct RenderPassInfo
    {
        Format rtFormats[8];
        std::uint32_t rtCount;
        Format dsFormat = Format::Undefined;
        std::uint32_t sampleCount;
    };

    struct PipelineState
    {
        ShaderProgram* program = nullptr;

        Topology topology = Topology::TriangleList;
        struct Rasterization
        {
            FillMode fillMode = FillMode::Fill;
            float lineWidth = 1.0f;
            CullMode cullMode = CullMode::None;
            bool reverseFrontFace = false;

            float depthBias = 0.0f;
            float depthBiasClamp = 0.0f;
            float depthBiasSlopeFactor = 0.0f;
            bool depthClipEnabled = false;
            bool scissorTestEnable = false;
        } rasterization;

        struct DepthStencil
        {
            bool depthTestEnable = false;
            bool depthWriteEnable = false;
            // Depth func
            bool stencilTestEnable = false;
            // FrontFace (StencilFailOp, DepthFailOp, PassOp, etc.)
            // BackFace (StencilFailOp, DepthFailOp, PassOp, etc.)
        } depthStencil;
    };
}