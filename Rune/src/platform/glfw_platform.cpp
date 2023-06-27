#include "platform/platform.hpp"

#include "internal_common.hpp"

#if defined(RUNE_PLATFORM_WINDOWS) || defined(RUNE_PLATFORM_LINUX)

    #include <GLFW/glfw3.h>

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
            std::pair<std::int32_t, std::int32_t> windowedSize{};
            std::pair<std::int32_t, std::int32_t> windowedPosition{};
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
            throw std::runtime_error("Failed to initialise GLFW platform!");
        }
    }

    void shutdown()
    {
        glfwTerminate();

        g_glfwInitialised = false;
    }
    #pragma region Time

    auto get_time() -> double
    {
        return glfwGetTime();
    }

    #pragma endregion

    #pragma region Windows

    auto create_window(std::int32_t width, std::int32_t height, std::string_view title) -> WindowHandle
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

    void set_window_size(WindowHandle window, std::int32_t width, std::int32_t height)
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);
        glfwSetWindowSize(glfwWindow, width, height);
    }

    void set_window_position(WindowHandle window, std::int32_t x, std::int32_t y)
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
        auto glfwKey = static_cast<std::uint16_t>(key);

        const auto* keyName = glfwGetKeyName(glfwKey, 0);
        return keyName;
    }

    bool is_key_down(WindowHandle window, Key key)
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);
        auto glfwKey = static_cast<std::uint16_t>(key);

        auto state = glfwGetKey(glfwWindow, glfwKey);
        return state != GLFW_RELEASE;
    }

    auto get_cursor_position(WindowHandle window) -> std::pair<double, double>
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);

        double x{ 0.0f };
        double y{ 0.0f };
        glfwGetCursorPos(glfwWindow, &x, &y);
        return { x, y };
    }

    bool is_mouse_button_down(WindowHandle window, Button button)
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);
        auto glfwButton = static_cast<std::uint8_t>(button);

        auto state = glfwGetMouseButton(glfwWindow, glfwButton);
        return state != GLFW_RELEASE;
    }

    bool is_gamepad_present(Gamepad gamepad)
    {
        auto glfwGamepad = static_cast<std::uint8_t>(gamepad);
        return glfwJoystickIsGamepad(glfwGamepad);
    }

    auto get_gamepad_name(Gamepad gamepad) -> std::string_view
    {
        auto glfwGamepad = static_cast<std::uint8_t>(gamepad);
        const auto* gamepadName = glfwGetGamepadName(glfwGamepad);
        return gamepadName;
    }

    bool is_gamepad_button_down(Gamepad gamepad, GamepadButton button)
    {
        auto glfwGamepad = static_cast<std::uint8_t>(gamepad);
        auto glfwButton = static_cast<std::uint8_t>(button);

        GLFWgamepadstate state{};
        glfwGetGamepadState(glfwGamepad, &state);
        return state.buttons[glfwButton] != GLFW_RELEASE;  // NOLINT
    }

    auto get_gamepad_axis_state(Gamepad gamepad, GamepadAxis axis) -> float
    {
        auto glfwGamepad = static_cast<std::uint8_t>(gamepad);
        auto glfwAxis = static_cast<std::uint8_t>(axis);

        GLFWgamepadstate state{};
        glfwGetGamepadState(glfwGamepad, &state);
        return state.axes[glfwAxis];  // NOLINT
    }

    #pragma endregion

}

#endif