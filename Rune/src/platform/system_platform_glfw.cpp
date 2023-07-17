#include "platform/system_platform_glfw.hpp"

#include "common_internal.hpp"
#include "events/system_events.hpp"

#include <GLFW/glfw3.h>
#if defined(RUNE_PLATFORM_WINDOWS)
    #define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(RUNE_PLATFORM_LINUX)
    #define GLFW_EXPOSE_NATIVE_WAYLAND
#endif
#define GLFW_NATIVE_INCLUDE_NONE
#include <GLFW/glfw3native.h>

namespace rune
{
    struct SystemPlatformGLFW::Pimpl
    {
        bool glfwInitialised{ false };
    };

    void window_size_callback(GLFWwindow* window, i32 width, i32 height);
    void cursor_pos_callback(GLFWwindow* window, double x, double y);
    void mouse_btn_callback(GLFWwindow* window, int btn, int action, int mods);

    SystemPlatformGLFW::SystemPlatformGLFW() : m_pimpl(new Pimpl) {}

    SystemPlatformGLFW::~SystemPlatformGLFW() = default;

    void SystemPlatformGLFW::initialise()
    {
        RUNE_ASSERT(!m_pimpl->glfwInitialised);

        if (!glfwInit())
        {
            RUNE_THROW_EX("Failed to initialise GLFW platform!");
        }

        m_pimpl->glfwInitialised = true;

        LOG_INFO("Platform (GLFW) system initialised.");
    }

    void SystemPlatformGLFW::update()
    {
        glfwPollEvents();
    }

    void SystemPlatformGLFW::shutdown()
    {
        glfwTerminate();
        m_pimpl->glfwInitialised = true;

        LOG_INFO("Platform (GLFW) system has shut down.");
    }

    f64 SystemPlatformGLFW::get_time() const
    {
        return glfwGetTime();
    }

    auto SystemPlatformGLFW::create_window(i32 width, i32 height, std::string_view title) -> WindowHandle
    {
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        auto* glfwWindow = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);

        glfwSetWindowUserPointer(glfwWindow, this);

        glfwSetWindowSizeCallback(glfwWindow, window_size_callback);
        glfwSetCursorPosCallback(glfwWindow, cursor_pos_callback);
        glfwSetMouseButtonCallback(glfwWindow, mouse_btn_callback);

        // Trigger some callbacks
        window_size_callback(glfwWindow, width, height);

        return glfwWindow;
    }

    void SystemPlatformGLFW::destroy_window(WindowHandle window)
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);
        glfwDestroyWindow(glfwWindow);
    }

    void SystemPlatformGLFW::show_window(WindowHandle window)
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);
        glfwShowWindow(glfwWindow);
    }

    void SystemPlatformGLFW::hide_window(WindowHandle window)
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);
        glfwHideWindow(glfwWindow);
    }

    bool SystemPlatformGLFW::has_window_requested_close(WindowHandle window)
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);
        bool closeRequested = glfwWindowShouldClose(glfwWindow);
        return closeRequested;
    }

    auto SystemPlatformGLFW::get_platform_display_handle() -> void*
    {
#if defined(RUNE_PLATFORM_WINDOWS)
        return GetModuleHandle(nullptr);
#elif defined(RUNE_PLATFORM_LINUX)
        return glfwGetWaylandDisplay();
#endif
    }

    auto SystemPlatformGLFW::get_window_platform_handle(WindowHandle window) -> void*
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);
#if defined(RUNE_PLATFORM_WINDOWS)
        return glfwGetWin32Window(glfwWindow);
#elif defined(RUNE_PLATFORM_LINUX)
        return glfwGetWaylandWindow(glfwWindow);
#endif
    }

    glm::ivec2 SystemPlatformGLFW::get_window_size(WindowHandle window)
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);
        int width{};
        int height{};
        glfwGetWindowSize(glfwWindow, &width, &height);
        return { width, height };
    }

    glm::ivec2 SystemPlatformGLFW::get_window_size_pixels(WindowHandle window)
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);
        int width{};
        int height{};
        glfwGetFramebufferSize(glfwWindow, &width, &height);
        return { width, height };
    }

    glm::ivec2 SystemPlatformGLFW::get_window_position(WindowHandle window)
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);
        int x{};
        int y{};
        glfwGetWindowPos(glfwWindow, &x, &y);
        return { x, y };
    }

    void SystemPlatformGLFW::set_window_size(WindowHandle window, i32 width, i32 height)
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);
        glfwSetWindowSize(glfwWindow, width, height);
    }

    void SystemPlatformGLFW::set_window_position(WindowHandle window, i32 x, i32 y)
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);
        glfwSetWindowPos(glfwWindow, x, y);
    }

    void SystemPlatformGLFW::set_window_title(WindowHandle window, std::string_view title)
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);
        glfwSetWindowTitle(glfwWindow, title.data());
    }

    void SystemPlatformGLFW::set_window_windowed(WindowHandle window)
    {
        RUNE_UNUSED(window);
        // #TODO: Implement SystemPlatformGLFW::set_window_windowed()
#if 0
        auto* glfwWindow = static_cast<GLFWwindow*>(window);
        const auto& windowData = g_windowData[glfwWindow];
        const auto x = windowData.windowedPosition.first;
        const auto y = windowData.windowedPosition.second;
        const auto width = windowData.windowedSize.first;
        const auto height = windowData.windowedSize.second;

        glfwSetWindowMonitor(glfwWindow, nullptr, x, y, width, height, 0);
#endif
    }

    void SystemPlatformGLFW::set_window_fullscreen(WindowHandle window)
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);

        auto* monitor = glfwGetPrimaryMonitor();
        const auto* mode = glfwGetVideoMode(monitor);

        glfwSetWindowMonitor(glfwWindow, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }

    std::string_view SystemPlatformGLFW::get_key_name(Key key)
    {
        auto glfwKey = static_cast<u16>(key);

        const auto* keyName = glfwGetKeyName(glfwKey, 0);
        return keyName;
    }

    bool SystemPlatformGLFW::is_key_down(WindowHandle window, Key key)
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);
        auto glfwKey = static_cast<u16>(key);

        auto state = glfwGetKey(glfwWindow, glfwKey);
        return state != GLFW_RELEASE;
    }

    bool SystemPlatformGLFW::is_mouse_button_down(WindowHandle window, Button button)
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);
        auto glfwButton = static_cast<u8>(button);

        auto state = glfwGetMouseButton(glfwWindow, glfwButton);
        return state != GLFW_RELEASE;
    }

    glm::ivec2 SystemPlatformGLFW::get_cursor_position(WindowHandle window)
    {
        auto* glfwWindow = static_cast<GLFWwindow*>(window);

        f64 x{ 0.0f };
        f64 y{ 0.0f };
        glfwGetCursorPos(glfwWindow, &x, &y);
        return { x, y };
    }

    bool SystemPlatformGLFW::is_gamepad_present(Gamepad gamepad)
    {
        auto glfwGamepad = static_cast<u8>(gamepad);
        return glfwJoystickIsGamepad(glfwGamepad);
    }

    std::string_view SystemPlatformGLFW::get_gamepad_name(Gamepad gamepad)
    {
        auto glfwGamepad = static_cast<u8>(gamepad);
        const auto* gamepadName = glfwGetGamepadName(glfwGamepad);
        return gamepadName;
    }

    bool SystemPlatformGLFW::is_gamepad_button_down(Gamepad gamepad, GamepadButton button)
    {
        auto glfwGamepad = static_cast<u8>(gamepad);
        auto glfwButton = static_cast<u8>(button);

        GLFWgamepadstate state{};
        glfwGetGamepadState(glfwGamepad, &state);
        return state.buttons[glfwButton] != GLFW_RELEASE;  // NOLINT
    }

    f32 SystemPlatformGLFW::get_gamepad_axis_state(Gamepad gamepad, GamepadAxis axis)
    {
        auto glfwGamepad = static_cast<u8>(gamepad);
        auto glfwAxis = static_cast<u8>(axis);

        GLFWgamepadstate state{};
        glfwGetGamepadState(glfwGamepad, &state);
        return state.axes[glfwAxis];  // NOLINT
    }

    void window_size_callback(GLFWwindow* window, i32 width, i32 height)
    {
        Event event{};
        event.type = EventType::WindowSize;
        event.context = window;
        event.payload.int32[0] = width;
        event.payload.int32[1] = height;
        Engine::get().get_system<SystemEvents>()->post_event(event);
    }

    void cursor_pos_callback(GLFWwindow* window, double x, double y)
    {
        Event event{};
        event.type = EventType::InputCursorPos;
        event.context = window;
        event.payload.float32[0] = f32(x);
        event.payload.float32[1] = f32(y);
        Engine::get().get_system<SystemEvents>()->post_event(event);
    }

    void mouse_btn_callback(GLFWwindow* window, int btn, int action, int mods)
    {
        RUNE_UNUSED(mods);

        if (action == GLFW_REPEAT)
        {
            return;  // Ignore repeat input
        }

        Event event{};
        event.type = EventType::InputButton;
        event.context = window;
        event.payload.uint32[0] = btn;
        event.payload.uint32[1] = action == GLFW_PRESS;
        Engine::get().get_system<SystemEvents>()->post_event(event);
    }

}