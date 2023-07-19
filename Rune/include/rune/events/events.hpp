#pragma once

#include "rune/common.hpp"

#include <queue>

namespace rune
{
    enum class EventType : u8
    {
        None,
        WindowCreate,
        WindowDestroy,
    };

    struct Event
    {
        EventType type{ EventType::None };  // 1 byte
        void* context{ nullptr };           // 8 bytes
        union Payload
        {
            u8 uint8[16];   // 1 byte x 16
            u16 uint16[8];  // 2 bytes x 8
            u32 uint32[4];  // 4 bytes x 4
            u64 uint64[2];  // 8 bytes x 8

            i8 int8[16];   // 1 byte x 16
            i16 int16[8];  // 2 bytes x 8
            i32 int32[4];  // 4 bytes x 4
            i64 int64[2];  // 8 bytes x 2

            f32 float32[4];  // 4 bytes x 4
            f64 float64[2];  // 8 bytes x 2

            void* ptr[2];  // 8 bytes x 2
        };
        Payload payload{};  // 16 bytes
    };

    // #TODO: Events - Make thread safe.
    class Events
    {
    public:
        void update();

        using EventCallbackFn = std::function<bool(const Event& event)>;
        void subscribe(EventCallbackFn&& callback);

        void post(const Event& event);

    private:
        std::vector<EventCallbackFn> m_subscribers{};
        std::queue<Event> m_eventQueue{};
    };

#define EVENT_WINDOW_CREATE(_windowHandle)                         \
    Event                                                          \
    {                                                              \
        .type = EventType::WindowCreate, .context = _windowHandle, \
    }

#define EVENT_WINDOW_DESTROY(_windowHandle)                        \
    Event                                                          \
    {                                                              \
        .type = EventType::WindowCreate, .context = _windowHandle, \
    }

#define EVENT_WINDOW_SIZE(_windowHandle, _w, _h)                                                               \
    Event                                                                                                      \
    {                                                                                                          \
        .type = EventType::WindowCreate, .context = _windowHandle, .payload = {.int32 = { i32(_w), i32(_h) } } \
    }
}