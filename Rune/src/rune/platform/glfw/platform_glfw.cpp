#include "rune/platform/platform.hpp"

#include "rune/debug/log_engine.hpp"
#include "rune/debug/assert_engine.hpp"

#ifdef RUNE_PLATFORM_WINDOWS

    #define GLFW_INCLUDE_NONE
    #include <GLFW/glfw3.h>

namespace rune::platform
{
    struct PlatformData
    {
        std::unordered_map<GLFWwindow*, Window> windowMap{};
    };
    static Owned<PlatformData> s_platform{ nullptr };

    bool initialise()
    {
        RUNE_ENG_ASSERT(s_platform == nullptr);
        s_platform = create_owned<PlatformData>();

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

        s_platform->windowMap.emplace(glfwWindow, Window(glfwWindow));

        return &s_platform->windowMap.at(glfwWindow);
    }

    void destroy_window(WindowPtr window)
    {
        RUNE_ENG_ASSERT(s_platform != nullptr);

        auto* glfwWindow = (GLFWwindow*)window->handle();

        auto it = s_platform->windowMap.find(glfwWindow);
        if (it == s_platform->windowMap.end())
            return;

        glfwDestroyWindow(glfwWindow);
        s_platform->windowMap.erase(glfwWindow);
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

}

#endif