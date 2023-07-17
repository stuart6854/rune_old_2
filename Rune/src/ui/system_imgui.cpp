#include "ui/system_imgui.hpp"

#include "common_internal.hpp"
#include "core/engine.hpp"
#include "rendering/system_renderer.hpp"

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
        auto* systemRenderer = Engine::get().get_system<SystemRenderer>();

        // Load fonts
        io.Fonts->AddFontDefault();

        {
            i32 width{};
            i32 height{};
            u8* data{ nullptr };
            io.Fonts->GetTexDataAsAlpha8(&data, &width, &height);
            // TODO: Create texture
            auto device = systemRenderer->get_device();
            gfx::TextureInfo textureInfo{
                .usage = gfx::TextureUsage::eTexture,
                .type = gfx::TextureType::e2D,
                .width = u32(width),
                .height = u32(height),
                .format = gfx::Format::eR8,
                .mipLevels = 1,
            };
            if (!gfx::create_texture(m_fontTexture, device, textureInfo))
            {
                // #TODO: Handle error
                LOG_ERROR("SystemImGui - Failed to create font texture.");
            }

            // Create & Submit command list to upload texture data
            // Create staging buffer
            gfx::BufferInfo stagingBufferInfo{
                .type = gfx::BufferType::eUpload,
                .size = sizeof(std::uint8_t) * (width * height),
            };
            gfx::BufferHandle stagingBufferHandle{};
            if (!gfx::create_buffer(stagingBufferHandle, device, stagingBufferInfo))
            {
                throw std::runtime_error("Failed to create GFX staging buffer!");
            }

            void* stagingBufferPtr{ nullptr };
            if (gfx::map_buffer(stagingBufferHandle, stagingBufferPtr))
            {
                std::memcpy(stagingBufferPtr, data, stagingBufferInfo.size);
                gfx::unmap_buffer(stagingBufferHandle);
            }

            gfx::CommandListHandle uploadCommandListHandle{};
            if (!gfx::create_command_list(uploadCommandListHandle, device, 0))
            {
                throw std::runtime_error("Failed to create GFX upload command list!");
            }

            gfx::begin(uploadCommandListHandle);

            gfx::transition_texture(uploadCommandListHandle, m_fontTexture, gfx::TextureState::eUndefined, gfx::TextureState::eUploadDst);
            gfx::copy_buffer_to_texture(uploadCommandListHandle, stagingBufferHandle, m_fontTexture);
            gfx::transition_texture(uploadCommandListHandle, m_fontTexture, gfx::TextureState::eUploadDst, gfx::TextureState::eShaderRead);

            gfx::end(uploadCommandListHandle);

            gfx::SubmitInfo submitInfo{ .commandList = uploadCommandListHandle, .waitSemaphoreHandle = {} };
            gfx::FenceHandle fenceHandle{};
            gfx::submit_command_list(submitInfo, &fenceHandle, nullptr);

            gfx::wait_on_fence(fenceHandle);
            gfx::destroy_command_list(device, uploadCommandListHandle);
            gfx::destroy_buffer(stagingBufferHandle);

            gfx::DescriptorSetInfo setInfo{
                .bindings = {{gfx::DescriptorType::eTexture, 1, gfx::ShaderStageFlags_Fragment},},
            };
            if (!gfx::create_descriptor_set(m_fontTextureSet, device, setInfo))
            {
                LOG_ERROR("SystemImGui - Failed to create font texture descriptor set.");
            }

            gfx::SamplerInfo samplerInfo{
                .addressMode = gfx::SamplerAddressMode::eRepeat,
                .filterMode = gfx::SamplerFilterMode::eLinear,
            };
            gfx::SamplerHandle samplerHandle{};
            if (!gfx::create_sampler(samplerHandle, device, samplerInfo))
            {
                throw std::runtime_error("Failed to create GFX sampler!");
            }

            gfx::bind_texture_to_descriptor_set(m_fontTextureSet, 0, m_fontTexture, samplerHandle);
        }

        io.Fonts->SetTexID((void*)&m_fontTextureSet);
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