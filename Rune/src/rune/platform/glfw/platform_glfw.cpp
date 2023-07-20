#include "rune/platform/platform.hpp"

#include "rune/debug/log_engine.hpp"
#include "rune/debug/assert_engine.hpp"

#ifdef RUNE_PLATFORM_WINDOWS

    #define GLFW_INCLUDE_NONE
    #include <GLFW/glfw3.h>
    #ifdef RUNE_PLATFORM_WINDOWS
        #define GLFW_EXPOSE_NATIVE_WIN32
        #define WIN32_LEAN_AND_MEAN
        #define NOMINMAX
        #include <windows.h>
    #endif
    #define GLFW_NATIVE_INCLUDE_NONE
    #include <glfw/glfw3native.h>

namespace rune::platform
{
    struct PlatformData
    {
        Events* events{ nullptr };
        std::unordered_map<GLFWwindow*, Window> windowMap{};
    };
    static Owned<PlatformData> s_platform{ nullptr };

    void on_window_size(GLFWwindow* window, double w, double h);

    bool initialise(Events& events)
    {
        RUNE_ENG_ASSERT(s_platform == nullptr);
        s_platform = create_owned<PlatformData>();

        s_platform->events = &events;

        if (!glfwInit())
        {
            s_platform = nullptr;
            return false;
        }

        RUNE_ENG_INFO("Platform GLFW initialised.");

        return true;
    }

    bool shutdown()
    {
        RUNE_ENG_ASSERT(s_platform != nullptr);
        for (auto& [handle, window] : s_platform->windowMap)
        {
            glfwDestroyWindow((GLFWwindow*)handle);
        }
        s_platform->windowMap.clear();

        glfwTerminate();

        s_platform = nullptr;

        RUNE_ENG_INFO("Platform GLFW shutdown.");

        return true;
    }

    void update()
    {
        RUNE_ENG_ASSERT(s_platform != nullptr);
        glfwPollEvents();
    }

    auto create_window(std::string_view title, i32 width, i32 height) -> WindowPtr
    {
        RUNE_ENG_ASSERT(s_platform != nullptr);

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        auto glfwWindow = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
        glfwSetWindowUserPointer(glfwWindow, s_platform.get());

        s_platform->windowMap.emplace(glfwWindow, Window(glfwWindow));
        WindowPtr window = &s_platform->windowMap.at(glfwWindow);

        s_platform->events->post(EVENT_WINDOW_CREATE(window));

        RUNE_ENG_DEBUG("Platform: Window created. title={}, size={}", title, window->size());

        return window;
    }

    void destroy_window(WindowPtr window)
    {
        RUNE_ENG_ASSERT(s_platform != nullptr);

        auto* glfwWindow = (GLFWwindow*)window->handle();

        auto it = s_platform->windowMap.find(glfwWindow);
        if (it == s_platform->windowMap.end())
            return;

        s_platform->events->post(EVENT_WINDOW_DESTROY(window));
        glfwDestroyWindow(glfwWindow);
        s_platform->windowMap.erase(glfwWindow);

        RUNE_ENG_DEBUG("Platform: Window destroyed.");
    }

    Window::Window(void* handle) : m_handle(handle) {}

    auto Window::handle() const -> void*
    {
        return m_handle;
    }

    auto Window::title() const -> std::string_view
    {
        return m_title;
    }

    auto Window::size() const -> glm::ivec2
    {
        return get_window_size((WindowPtr)this);
    }

    auto Window::fb_size() const -> glm::ivec2
    {
        return get_window_fb_size((WindowPtr)this);
    }

    auto Window::native_display_handle() const -> void*
    {
        return get_window_native_display_handle((WindowPtr)this);
    }

    auto Window::native_surface_handle() const -> void*
    {
        return get_window_native_surface_handle((WindowPtr)this);
    }

    bool Window::close_requested()
    {
        return platform::get_window_close_requested((WindowPtr)this);
    }

    auto get_window_title(WindowPtr window) -> std::string_view
    {
        return window->title();
    }

    auto get_window_size(WindowPtr window) -> glm::ivec2
    {
        glm::ivec2 size{};
        glfwGetWindowSize((GLFWwindow*)window->handle(), &size.x, &size.y);
        return size;
    }

    auto get_window_fb_size(WindowPtr window) -> glm::ivec2
    {
        glm::ivec2 size{};
        glfwGetFramebufferSize((GLFWwindow*)window->handle(), &size.x, &size.y);
        return size;
    }

    auto get_window_native_display_handle(WindowPtr window) -> void*
    {
    #ifdef RUNE_PLATFORM_WINDOWS
        RUNE_UNUSED(window);
        return GetModuleHandle(nullptr);
    #endif
    }

    auto get_window_native_surface_handle(WindowPtr window) -> void*
    {
    #ifdef RUNE_PLATFORM_WINDOWS
        return glfwGetWin32Window((GLFWwindow*)window->handle());
    #endif
    }

    bool get_window_close_requested(WindowPtr window)
    {
        return glfwWindowShouldClose((GLFWwindow*)window->handle());
    }

    void on_window_size(GLFWwindow* glfwWindow, double w, double h)
    {
        auto* platform = static_cast<PlatformData*>(glfwGetWindowUserPointer(glfwWindow));

        auto& window = platform->windowMap.at(glfwWindow);
        platform->events->post(EVENT_WINDOW_SIZE(&window, w, h));
    }
}

#endif