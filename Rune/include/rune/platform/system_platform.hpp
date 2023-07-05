#pragma once

#include "common.hpp"
#include "input_enums.hpp"
#include "core/platform_detection.hpp"
#include "core/system.hpp"

#include <glm/ext/vector_int2.hpp>

// #TODO: Get system/hardware info (Platform name, CPU cores/threads, RAM, ) - Linux=sysinfo, Windows=GetSystemInfo()

namespace rune
{
    using WindowHandle = void*;

    class SystemPlatform : public ISystem
    {
    public:
        virtual auto get_time() const -> f64 = 0;

#pragma region Windows

        virtual auto create_window(i32 width, i32 height, std::string_view title) -> WindowHandle = 0;
        virtual void destroy_window(WindowHandle window) = 0;

        virtual void show_window(WindowHandle window) = 0;
        virtual void hide_window(WindowHandle window) = 0;

        virtual bool has_window_requested_close(WindowHandle window) = 0;

        /* Window=HINSTANCE, Wayland=wl_display */
        virtual auto get_platform_display_handle() -> void* = 0;
        /* Windows=HWND, Wayland=wl_surface */
        virtual auto get_window_platform_handle(WindowHandle window) -> void* = 0;

        virtual auto get_window_size(WindowHandle window) -> glm::ivec2 = 0;
        virtual auto get_window_size_pixels(WindowHandle window) -> glm::ivec2 = 0;
        virtual auto get_window_position(WindowHandle window) -> glm::ivec2 = 0;

        virtual void set_window_size(WindowHandle window, i32 width, i32 height) = 0;
        virtual void set_window_position(WindowHandle window, i32 x, i32 y) = 0;
        virtual void set_window_title(WindowHandle window, std::string_view title) = 0;

        virtual void set_window_windowed(WindowHandle window) = 0;
        virtual void set_window_fullscreen(WindowHandle window) = 0;

#pragma endregion

#pragma region Input

        virtual auto get_key_name(Key key) -> std::string_view = 0;
        virtual bool is_key_down(WindowHandle window, Key key) = 0;

        virtual bool is_mouse_button_down(WindowHandle window, Button button) = 0;

        virtual auto get_cursor_position(WindowHandle window) -> glm::ivec2 = 0;

        virtual bool is_gamepad_present(Gamepad gamepad) = 0;
        virtual auto get_gamepad_name(Gamepad gamepad) -> std::string_view = 0;
        virtual bool is_gamepad_button_down(Gamepad gamepad, GamepadButton button) = 0;
        virtual auto get_gamepad_axis_state(Gamepad gamepad, GamepadAxis axis) -> f32 = 0;

#pragma endregion
    };
}