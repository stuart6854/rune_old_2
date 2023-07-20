#pragma once

#include "rune/common.hpp"
#include "common.hpp"

namespace rune::rhi
{
    class Device;

    class Fence
    {
    public:
        static auto create(Shared<Device> device, u64 initialValue = 0) -> Owned<Fence>;

        virtual ~Fence() = default;

        virtual auto completed_value() const -> u64 = 0;

        virtual void signal(u64 value) = 0;
        virtual void wait(u64 value) = 0;
    };
}