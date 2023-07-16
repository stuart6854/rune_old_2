#include "ui/system_imgui.hpp"

#include "common_internal.hpp"

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
        // #TODO: Plug into platform backend
        // #TODO: Plug into renderer backend

        // Load fonts
        io.Fonts->AddFontDefault();

        // TODO: Upload fonts to renderer
    }

    void SystemImGui::update()
    {
        ImGui::NewFrame();
        ImGui::Render();
        auto* drawData = ImGui::GetDrawData();
        RUNE_UNUSED(drawData);
        // #TODO: Render `drawData`
    }

    void SystemImGui::shutdown()
    {
        // #TODO: Unplug from platform backend
        // #TODO: Unplug from renderer backend

        ImGui::DestroyContext();
    }

}