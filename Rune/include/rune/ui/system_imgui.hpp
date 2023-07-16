#pragma once

#include "common.hpp"
#include "core/system.hpp"
#include "events/system_events.hpp"

namespace rune
{
    class SystemImGui : public ISystem
    {
    public:
        void initialise() override;
        void update() override;
        void shutdown() override;

    private:
        static void receive_event(const Event& event);

    private:
    };
}