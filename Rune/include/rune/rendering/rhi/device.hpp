#pragma once

#include "buffer.hpp"

namespace rune::rhi
{
    class CommandList;
    class Fence;

    class Device
    {
    public:
        static auto create() -> Owned<Device>;

        virtual void submit(const std::vector<CommandList*>& cmdLists, Fence* fence = nullptr, u64 signalValue = 0) = 0;
    };

}