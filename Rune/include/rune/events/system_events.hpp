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
            i32 int32[4];
            i64 int64[2];

            u32 uint32[4];
            u64 uint64[2];

            f32 float32[4];
            f64 float64[2];
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