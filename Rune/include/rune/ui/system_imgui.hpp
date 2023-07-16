#pragma once

#include "common.hpp"
#include "core/system.hpp"

namespace rune
{
    class SystemImGui : public ISystem
    {
    public:
        void initialise() override;
        void update() override;
        void shutdown() override;

    private:
    };
}