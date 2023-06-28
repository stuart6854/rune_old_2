#include "platform/platform.hpp"

#if defined(RUNE_PLATFORM_WINDOWS) || defined(RUNE_PLATFORM_LINUX)

    #include "internal_common.hpp"

    #include <GLFW/glfw3.h>
    #if defined(RUNE_PLATFORM_WINDOWS)
        #define GLFW_EXPOSE_NATIVE_WIN32
    #endif
    #define GLFW_NATIVE_INCLUDE_NONE
    #include <GLFW/glfw3native.h>

    #include <utility>
    #include <unordered_map>

namespace rune::platform
{
    namespace
    {
        bool g_glfwInitialised{ false };  // NOLINT

        struct WindowData
        {
            GLFWwindow* handle{ nullptr };
            std::pair<i32, i32> windowedSize{};
            std::pair<i32, i32> windowedPosition{};
        };
        std::unordered_map<WindowHandle, WindowData> g_windowData{};  // NOLINT
    }

    void initialise()
    {
        if (g_glfwInitialised)
        {
            return;
        }

        g_glfwInitialised = glfwInit();
        if (!g_glfwInitialised)
        {
            RUNE_THROW_EX("Failed to initialise GLFW platform!");
        }
    }

    void shutdown()
    {
        glfwTerminate();

        g_glfwInitialised = false;
    }

    void update()
    {
        glfwPollEvents();
    }

    #pragma region Time

    auto get_time() -> f64
    {
        return glfwGetTime();
    }

    #pragma endregion

    #pragma region Windows

    auto create_window(i32 width, i32 height, std::string_view title) -> WindowHandle
    {
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        auto* glfwWindow = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);

        auto& windowData = g_windowData[glfwWindow];
        windowData.handle = glfwWindow;
        windowData.windowedSize = { width, height };

        return glfwWindow;
    }

    void destroy_window(WindowHandle window)
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);
        glfwDestroyWindow(glfwWindow);
    }

    auto get_window_platform_handle(WindowHandle window) -> void*
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);
    #if defined(RUNE_PLATFORM_WINDOWS)
        return glfwGetWin32Window(glfwWindow);
    #elif defined(RUNE_PLATFORM_LINUX)
        #error X11 or Wayland?
    #endif
    }

    bool has_window_requested_close(WindowHandle window)
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);
        bool closeRequested = glfwWindowShouldClose(glfwWindow);
        return closeRequested;
    }

    void show_window(WindowHandle window)
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);
        glfwShowWindow(glfwWindow);
    }

    void hide_window(WindowHandle window)
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);
        glfwHideWindow(glfwWindow);
    }

    auto get_window_size(WindowHandle window) -> glm::i32vec2
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);
        int width{};
        int height{};
        glfwGetWindowSize(glfwWindow, &width, &height);
        return { width, height };
    }

    auto get_window_size_pixels(WindowHandle window) -> glm::i32vec2
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);
        int width{};
        int height{};
        glfwGetFramebufferSize(glfwWindow, &width, &height);
        return { width, height };
    }

    auto get_window_position(WindowHandle window) -> glm::i32vec2
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);
        int x{};
        int y{};
        glfwGetWindowPos(glfwWindow, &x, &y);
        return { x, y };
    }

    void set_window_size(WindowHandle window, i32 width, i32 height)
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);
        glfwSetWindowSize(glfwWindow, width, height);
    }

    void set_window_position(WindowHandle window, i32 x, i32 y)
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);
        glfwSetWindowPos(glfwWindow, x, y);
    }

    void set_window_title(WindowHandle window, std::string_view title)
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);
        glfwSetWindowTitle(glfwWindow, title.data());
    }

    void set_window_windowed(WindowHandle window)
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);

        const auto& windowData = g_windowData[glfwWindow];
        const auto x = windowData.windowedPosition.first;
        const auto y = windowData.windowedPosition.second;
        const auto width = windowData.windowedSize.first;
        const auto height = windowData.windowedSize.second;

        glfwSetWindowMonitor(glfwWindow, nullptr, x, y, width, height, 0);
    }

    void set_window_fullscreen(WindowHandle window)
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);

        auto* monitor = glfwGetPrimaryMonitor();
        const auto* mode = glfwGetVideoMode(monitor);

        glfwSetWindowMonitor(glfwWindow, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }

    #pragma endregion

    #pragma region Input

    auto get_key_name(Key key) -> std::string_view
    {
        auto glfwKey = static_cast<u16>(key);

        const auto* keyName = glfwGetKeyName(glfwKey, 0);
        return keyName;
    }

    bool is_key_down(WindowHandle window, Key key)
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);
        auto glfwKey = static_cast<u16>(key);

        auto state = glfwGetKey(glfwWindow, glfwKey);
        return state != GLFW_RELEASE;
    }

    auto get_cursor_position(WindowHandle window) -> std::pair<f64, f64>
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);

        f64 x{ 0.0f };
        f64 y{ 0.0f };
        glfwGetCursorPos(glfwWindow, &x, &y);
        return { x, y };
    }

    bool is_mouse_button_down(WindowHandle window, Button button)
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);
        auto glfwButton = static_cast<u8>(button);

        auto state = glfwGetMouseButton(glfwWindow, glfwButton);
        return state != GLFW_RELEASE;
    }

    bool is_gamepad_present(Gamepad gamepad)
    {
        auto glfwGamepad = static_cast<u8>(gamepad);
        return glfwJoystickIsGamepad(glfwGamepad);
    }

    auto get_gamepad_name(Gamepad gamepad) -> std::string_view
    {
        auto glfwGamepad = static_cast<u8>(gamepad);
        const auto* gamepadName = glfwGetGamepadName(glfwGamepad);
        return gamepadName;
    }

    bool is_gamepad_button_down(Gamepad gamepad, GamepadButton button)
    {
        auto glfwGamepad = static_cast<u8>(gamepad);
        auto glfwButton = static_cast<u8>(button);

        GLFWgamepadstate state{};
        glfwGetGamepadState(glfwGamepad, &state);
        return state.buttons[glfwButton] != GLFW_RELEASE;  // NOLINT
    }

    auto get_gamepad_axis_state(Gamepad gamepad, GamepadAxis axis) -> f32
    {
        auto glfwGamepad = static_cast<u8>(gamepad);
        auto glfwAxis = static_cast<u8>(axis);

        GLFWgamepadstate state{};
        glfwGetGamepadState(glfwGamepad, &state);
        return state.axes[glfwAxis];  // NOLINT
    }

    #pragma endregion

}

#endif