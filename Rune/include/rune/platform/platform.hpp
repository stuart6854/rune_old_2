#pragma once

#include "rune/common.hpp"
#include "rune/events/events.hpp"

#include <glm/ext/vector_int2.hpp>

#include <string_view>

namespace rune::platform
{
    bool initialise(Events& events);
    bool shutdown();

    void update();

    class Window
    {
    public:
        explicit Window(void* handle);
        ~Window() = default;

        auto handle() const -> void*;

        auto title() const -> std::string_view;
        auto size() const -> glm::ivec2;
        auto fb_size() const -> glm::ivec2;

    private:
        void* m_handle{ nullptr };
        std::string m_title{};
    };
    using WindowPtr = Window*;

    auto create_window(std::string_view title, i32 width, i32 height) -> WindowPtr;
    void destroy_window(WindowPtr window);

    auto get_window_title(WindowPtr window) -> std::string_view;
    auto get_window_size(WindowPtr window) -> glm::ivec2;
    auto get_window_fb_size(WindowPtr window) -> glm::ivec2;
}