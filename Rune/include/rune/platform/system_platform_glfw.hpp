#pragma once

#include "system_platform.hpp"

#include <memory>

#if defined(RUNE_PLATFORM_WINDOWS) || defined(RUNE_PLATFORM_LINUX)
namespace rune
{
    class SystemPlatformGLFW : public SystemPlatform
    {
    public:
        SystemPlatformGLFW();
        ~SystemPlatformGLFW();

        void initialise() override;
        void update() override;
        void shutdown() override;

        auto get_time() const -> f64 override;

        auto create_window(i32 width, i32 height, std::string_view title) -> WindowHandle override;
        void destroy_window(WindowHandle window) override;

        void show_window(WindowHandle window) override;
        void hide_window(WindowHandle window) override;

        bool has_window_requested_close(WindowHandle window) override;

        auto get_platform_display_handle() -> void* override;
        auto get_window_platform_handle(WindowHandle window) -> void* override;

        auto get_window_size(WindowHandle window) -> glm::ivec2 override;
        auto get_window_size_pixels(WindowHandle window) -> glm::ivec2 override;
        auto get_window_position(WindowHandle window) -> glm::ivec2 override;

        void set_window_size(WindowHandle window, i32 width, i32 height) override;
        void set_window_position(WindowHandle window, i32 x, i32 y) override;
        void set_window_title(WindowHandle window, std::string_view title) override;
        void set_window_windowed(WindowHandle window) override;
        void set_window_fullscreen(WindowHandle window) override;

        auto get_key_name(Key key) -> std::string_view override;
        bool is_key_down(WindowHandle window, Key key) override;

        bool is_mouse_button_down(WindowHandle window, Button button) override;

        auto get_cursor_position(WindowHandle window) -> glm::ivec2 override;

        bool is_gamepad_present(Gamepad gamepad) override;
        auto get_gamepad_name(Gamepad gamepad) -> std::string_view override;
        bool is_gamepad_button_down(Gamepad gamepad, GamepadButton button) override;
        auto get_gamepad_axis_state(Gamepad gamepad, GamepadAxis axis) -> f32 override;

    private:
        struct Pimpl;
        std::unique_ptr<Pimpl> m_pimpl;
    };
}
#endif