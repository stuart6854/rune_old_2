#pragma once

#include "rune/common.hpp"

namespace rune::rhi
{
    enum class BufferType
    {
        Vertex,
        Index,
        Uniform,
        Storage,
    };

    struct BufferDecl
    {
        BufferType Type{};
        u64 Size{};
    };

    class Device;

    class Buffer
    {
    public:
        static auto create(Shared<Device> device, const BufferDecl& decl) -> Owned<Buffer>;
        inline static auto create_vertex(Shared<Device> device, u64 size) -> Owned<Buffer>;
        inline static auto create_index(Shared<Device> device, u64 size) -> Owned<Buffer>;

        virtual auto type() const -> BufferType = 0;
        virtual auto size() const -> u64 = 0;
    };

    auto Buffer::create_vertex(Shared<Device> device, u64 size) -> Owned<Buffer>
    {
        BufferDecl decl{
            .Type = BufferType::Vertex,
            .Size = size,
        };
        return create(device, decl);
    }

    auto Buffer::create_index(Shared<Device> device, u64 size) -> Owned<Buffer>
    {
        BufferDecl decl{
            .Type = BufferType::Index,
            .Size = size,
        };
        return create(device, decl);
    }

}