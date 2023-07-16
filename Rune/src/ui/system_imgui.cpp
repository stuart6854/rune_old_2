#include "ui/system_imgui.hpp"

#include "common_internal.hpp"
#include "core/engine.hpp"

#include <imgui.h>

namespace rune
{
    void SystemImGui::initialise()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        auto& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

        ImGui::StyleColorsDark();
        // ImGui::StyleColorsLight();

        // Setup backend
        auto* eventSystem = Engine::get().get_system<SystemEvents>();
        eventSystem->register_receiver([this](const Event& event) { receive_event(event); });
        // #TODO: Plug into renderer backend

        // Load fonts
        io.Fonts->AddFontDefault();

        // TODO: Upload fonts to renderer
    }

    void SystemImGui::update()
    {
        auto& io = ImGui::GetIO();
        io.DeltaTime = Engine::get().get_delta_time();

        ImGui::NewFrame();
        ImGui::Render();
        auto* drawData = ImGui::GetDrawData();
        RUNE_UNUSED(drawData);
        // #TODO: Render `drawData`
    }

    void SystemImGui::shutdown()
    {
        // #TODO: Unregister event receiver
        // #TODO: Unplug from renderer backend

        ImGui::DestroyContext();
    }

    void SystemImGui::receive_event(const Event& event)
    {
        auto* primaryWindow = Engine::get().get_primary_window();
        if (event.context != primaryWindow)
        {
            return;
        }

        auto& io = ImGui::GetIO();
        if (event.type == EventType::WindowSize)
        {
            io.DisplaySize.x = f32(event.payload.int32[0]);
            io.DisplaySize.y = f32(event.payload.int32[1]);
        }
        else if (event.type == EventType::InputButton)
        {
            auto btn = event.payload.uint32[0];
            auto isDown = event.payload.uint32[1];
            io.AddMouseButtonEvent(i32(btn), isDown);
        }
        else if (event.type == EventType::InputCursorPos)
        {
            auto x = event.payload.float32[0];
            auto y = event.payload.float32[1];
            io.AddMousePosEvent(x, y);
        }
    }

}