#pragma once

#include "common.hpp"
#include "core/system.hpp"

#include <queue>
#include <functional>

namespace rune
{
    enum class EventType : u64
    {
        WindowSize,
        InputKey,
        InputButton,
    };

    struct Event
    {
        EventType type{};
        void* context{};
        union Payload
        {
            i32 i32[4];
            i64 i64[2];

            u32 u32[4];
            u64 u64[2];

            f32 f32[4];
            f64 f64[2];
        } payload;
    };

    class SystemEvents : public ISystem
    {
    public:
        void initialise() override;
        void update() override;
        void shutdown() override;

        using ReceiveEventFunc = std::function<void(const Event&)>;
        void register_receiver(ReceiveEventFunc&& receiveEventFunc);

        void post_event(const Event& msg);

    private:
        std::vector<ReceiveEventFunc> m_receivers{};

        std::queue<Event> m_messages{};
    };
}