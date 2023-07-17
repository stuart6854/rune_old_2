#include "ui/system_imgui.hpp"

#include "common_internal.hpp"
#include "core/engine.hpp"
#include "rendering/system_renderer.hpp"

#include <imgui.h>

namespace rune
{
    // Taken from imgui_impl_vulkan.cpp
    static const std::vector<u32> ImGuiVertexShaderSpv = {
        0x07230203, 0x00010000, 0x00080001, 0x0000002e, 0x00000000, 0x00020011, 0x00000001, 0x0006000b, 0x00000001, 0x4c534c47, 0x6474732e,
        0x3035342e, 0x00000000, 0x0003000e, 0x00000000, 0x00000001, 0x000a000f, 0x00000000, 0x00000004, 0x6e69616d, 0x00000000, 0x0000000b,
        0x0000000f, 0x00000015, 0x0000001b, 0x0000001c, 0x00030003, 0x00000002, 0x000001c2, 0x00040005, 0x00000004, 0x6e69616d, 0x00000000,
        0x00030005, 0x00000009, 0x00000000, 0x00050006, 0x00000009, 0x00000000, 0x6f6c6f43, 0x00000072, 0x00040006, 0x00000009, 0x00000001,
        0x00005655, 0x00030005, 0x0000000b, 0x0074754f, 0x00040005, 0x0000000f, 0x6c6f4361, 0x0000726f, 0x00030005, 0x00000015, 0x00565561,
        0x00060005, 0x00000019, 0x505f6c67, 0x65567265, 0x78657472, 0x00000000, 0x00060006, 0x00000019, 0x00000000, 0x505f6c67, 0x7469736f,
        0x006e6f69, 0x00030005, 0x0000001b, 0x00000000, 0x00040005, 0x0000001c, 0x736f5061, 0x00000000, 0x00060005, 0x0000001e, 0x73755075,
        0x6e6f4368, 0x6e617473, 0x00000074, 0x00050006, 0x0000001e, 0x00000000, 0x61635375, 0x0000656c, 0x00060006, 0x0000001e, 0x00000001,
        0x61725475, 0x616c736e, 0x00006574, 0x00030005, 0x00000020, 0x00006370, 0x00040047, 0x0000000b, 0x0000001e, 0x00000000, 0x00040047,
        0x0000000f, 0x0000001e, 0x00000002, 0x00040047, 0x00000015, 0x0000001e, 0x00000001, 0x00050048, 0x00000019, 0x00000000, 0x0000000b,
        0x00000000, 0x00030047, 0x00000019, 0x00000002, 0x00040047, 0x0000001c, 0x0000001e, 0x00000000, 0x00050048, 0x0000001e, 0x00000000,
        0x00000023, 0x00000000, 0x00050048, 0x0000001e, 0x00000001, 0x00000023, 0x00000008, 0x00030047, 0x0000001e, 0x00000002, 0x00020013,
        0x00000002, 0x00030021, 0x00000003, 0x00000002, 0x00030016, 0x00000006, 0x00000020, 0x00040017, 0x00000007, 0x00000006, 0x00000004,
        0x00040017, 0x00000008, 0x00000006, 0x00000002, 0x0004001e, 0x00000009, 0x00000007, 0x00000008, 0x00040020, 0x0000000a, 0x00000003,
        0x00000009, 0x0004003b, 0x0000000a, 0x0000000b, 0x00000003, 0x00040015, 0x0000000c, 0x00000020, 0x00000001, 0x0004002b, 0x0000000c,
        0x0000000d, 0x00000000, 0x00040020, 0x0000000e, 0x00000001, 0x00000007, 0x0004003b, 0x0000000e, 0x0000000f, 0x00000001, 0x00040020,
        0x00000011, 0x00000003, 0x00000007, 0x0004002b, 0x0000000c, 0x00000013, 0x00000001, 0x00040020, 0x00000014, 0x00000001, 0x00000008,
        0x0004003b, 0x00000014, 0x00000015, 0x00000001, 0x00040020, 0x00000017, 0x00000003, 0x00000008, 0x0003001e, 0x00000019, 0x00000007,
        0x00040020, 0x0000001a, 0x00000003, 0x00000019, 0x0004003b, 0x0000001a, 0x0000001b, 0x00000003, 0x0004003b, 0x00000014, 0x0000001c,
        0x00000001, 0x0004001e, 0x0000001e, 0x00000008, 0x00000008, 0x00040020, 0x0000001f, 0x00000009, 0x0000001e, 0x0004003b, 0x0000001f,
        0x00000020, 0x00000009, 0x00040020, 0x00000021, 0x00000009, 0x00000008, 0x0004002b, 0x00000006, 0x00000028, 0x00000000, 0x0004002b,
        0x00000006, 0x00000029, 0x3f800000, 0x00050036, 0x00000002, 0x00000004, 0x00000000, 0x00000003, 0x000200f8, 0x00000005, 0x0004003d,
        0x00000007, 0x00000010, 0x0000000f, 0x00050041, 0x00000011, 0x00000012, 0x0000000b, 0x0000000d, 0x0003003e, 0x00000012, 0x00000010,
        0x0004003d, 0x00000008, 0x00000016, 0x00000015, 0x00050041, 0x00000017, 0x00000018, 0x0000000b, 0x00000013, 0x0003003e, 0x00000018,
        0x00000016, 0x0004003d, 0x00000008, 0x0000001d, 0x0000001c, 0x00050041, 0x00000021, 0x00000022, 0x00000020, 0x0000000d, 0x0004003d,
        0x00000008, 0x00000023, 0x00000022, 0x00050085, 0x00000008, 0x00000024, 0x0000001d, 0x00000023, 0x00050041, 0x00000021, 0x00000025,
        0x00000020, 0x00000013, 0x0004003d, 0x00000008, 0x00000026, 0x00000025, 0x00050081, 0x00000008, 0x00000027, 0x00000024, 0x00000026,
        0x00050051, 0x00000006, 0x0000002a, 0x00000027, 0x00000000, 0x00050051, 0x00000006, 0x0000002b, 0x00000027, 0x00000001, 0x00070050,
        0x00000007, 0x0000002c, 0x0000002a, 0x0000002b, 0x00000028, 0x00000029, 0x00050041, 0x00000011, 0x0000002d, 0x0000001b, 0x0000000d,
        0x0003003e, 0x0000002d, 0x0000002c, 0x000100fd, 0x00010038
    };
    // Taken from imgui_impl_vulkan.cpp
    static const std::vector<u32> ImGuiFragmentShaderSpv = {
        0x07230203, 0x00010000, 0x00080001, 0x0000001e, 0x00000000, 0x00020011, 0x00000001, 0x0006000b, 0x00000001, 0x4c534c47, 0x6474732e,
        0x3035342e, 0x00000000, 0x0003000e, 0x00000000, 0x00000001, 0x0007000f, 0x00000004, 0x00000004, 0x6e69616d, 0x00000000, 0x00000009,
        0x0000000d, 0x00030010, 0x00000004, 0x00000007, 0x00030003, 0x00000002, 0x000001c2, 0x00040005, 0x00000004, 0x6e69616d, 0x00000000,
        0x00040005, 0x00000009, 0x6c6f4366, 0x0000726f, 0x00030005, 0x0000000b, 0x00000000, 0x00050006, 0x0000000b, 0x00000000, 0x6f6c6f43,
        0x00000072, 0x00040006, 0x0000000b, 0x00000001, 0x00005655, 0x00030005, 0x0000000d, 0x00006e49, 0x00050005, 0x00000016, 0x78655473,
        0x65727574, 0x00000000, 0x00040047, 0x00000009, 0x0000001e, 0x00000000, 0x00040047, 0x0000000d, 0x0000001e, 0x00000000, 0x00040047,
        0x00000016, 0x00000022, 0x00000000, 0x00040047, 0x00000016, 0x00000021, 0x00000000, 0x00020013, 0x00000002, 0x00030021, 0x00000003,
        0x00000002, 0x00030016, 0x00000006, 0x00000020, 0x00040017, 0x00000007, 0x00000006, 0x00000004, 0x00040020, 0x00000008, 0x00000003,
        0x00000007, 0x0004003b, 0x00000008, 0x00000009, 0x00000003, 0x00040017, 0x0000000a, 0x00000006, 0x00000002, 0x0004001e, 0x0000000b,
        0x00000007, 0x0000000a, 0x00040020, 0x0000000c, 0x00000001, 0x0000000b, 0x0004003b, 0x0000000c, 0x0000000d, 0x00000001, 0x00040015,
        0x0000000e, 0x00000020, 0x00000001, 0x0004002b, 0x0000000e, 0x0000000f, 0x00000000, 0x00040020, 0x00000010, 0x00000001, 0x00000007,
        0x00090019, 0x00000013, 0x00000006, 0x00000001, 0x00000000, 0x00000000, 0x00000000, 0x00000001, 0x00000000, 0x0003001b, 0x00000014,
        0x00000013, 0x00040020, 0x00000015, 0x00000000, 0x00000014, 0x0004003b, 0x00000015, 0x00000016, 0x00000000, 0x0004002b, 0x0000000e,
        0x00000018, 0x00000001, 0x00040020, 0x00000019, 0x00000001, 0x0000000a, 0x00050036, 0x00000002, 0x00000004, 0x00000000, 0x00000003,
        0x000200f8, 0x00000005, 0x00050041, 0x00000010, 0x00000011, 0x0000000d, 0x0000000f, 0x0004003d, 0x00000007, 0x00000012, 0x00000011,
        0x0004003d, 0x00000014, 0x00000017, 0x00000016, 0x00050041, 0x00000019, 0x0000001a, 0x0000000d, 0x00000018, 0x0004003d, 0x0000000a,
        0x0000001b, 0x0000001a, 0x00050057, 0x00000007, 0x0000001c, 0x00000017, 0x0000001b, 0x00050085, 0x00000007, 0x0000001d, 0x00000012,
        0x0000001c, 0x0003003e, 0x00000009, 0x0000001d, 0x000100fd, 0x00010038
    };

    void create_or_resize_buffer(gfx::BufferHandle& bufferHandle, sizet& bufferSize, sizet newSize, gfx::BufferType bufferType)
    {
        auto* systemRenderer = Engine::get().get_system<SystemRenderer>();
        auto device = systemRenderer->get_device();

        if (bufferHandle)
        {
            gfx::destroy_buffer(bufferHandle);
        }

        gfx::BufferInfo bufferInfo{
            .type = bufferType,
            .size = newSize,
        };
        if (!gfx::create_buffer(bufferHandle, device, bufferInfo))
        {
            // #TODO: Handle error
            LOG_ERROR("SystemImGui - Failed to create buffer.");
        }

        bufferSize = newSize;
    }

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
        auto* systemRenderer = Engine::get().get_system<SystemRenderer>();
        systemRenderer->bind_custom_ui_render_func([this](gfx::CommandListHandle cmdList, u32 frameIndex)
                                                   { render_ui(cmdList, frameIndex); });

        // Load fonts
        io.Fonts->AddFontDefault();

        auto device = systemRenderer->get_device();

        // Create pipeline
        {
            gfx::GraphicsPipelineInfo pipelineInfo
            {
                .vertexCode = ImGuiVertexShaderSpv, .fragmentCode = ImGuiFragmentShaderSpv,
                .vertexInputBindings = {
                        gfx::VertexBinding("_", {
                            gfx::VertexAttribute("Position", 0, gfx::Format::eRG32 ),
                            gfx::VertexAttribute("UV", 1, gfx::Format::eRG32 ),
                            gfx::VertexAttribute("Color", 2, gfx::Format::eRGBA8 )
                        }),
                    },
                    .descriptorSets = {
                        gfx::DescriptorSetInfo({
                            gfx::DescriptorBindingInfo(gfx::DescriptorType::eTexture, 1, gfx::ShaderStageFlags_Fragment)
                        })
                    },
                    .constantBlock = { sizeof(glm::vec2) * 2, gfx::ShaderStageFlags_Vertex },
                    .depthTest = true,
                    .colorAttachments = { gfx::Format::eBGRA8 },
                    .depthAttachmentFormat = gfx::Format::eDepth24Stencil8,
                };
            if (!gfx::create_graphics_pipeline(m_pipeline, device, pipelineInfo))
            {
                // #TODO: Handle error
                LOG_ERROR("SystemImGui - Failed to create font texture.");
            }
        }

        {
            i32 width{};
            i32 height{};
            u8* data{ nullptr };
            io.Fonts->GetTexDataAsRGBA32(&data, &width, &height);
            sizet upload_size = width * height * 4 * sizeof(u8);

            gfx::TextureInfo textureInfo{
                .usage = gfx::TextureUsage::eTexture,
                .type = gfx::TextureType::e2D,
                .width = u32(width),
                .height = u32(height),
                .format = gfx::Format::eRGBA8,
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
                .size = upload_size,
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

        static bool s_showDemo = true;
        ImGui::ShowDemoWindow(&s_showDemo);

        //        ImGui::Begin("Hello Window");
        //        ImGui::End();
    }

    void SystemImGui::shutdown()
    {
        // #TODO: Unregister event receiver
        // #TODO: Unplug from renderer backend

        ImGui::DestroyContext();

        auto* systemRenderer = Engine::get().get_system<SystemRenderer>();
        gfx::wait_for_device_idle(systemRenderer->get_device());

        // #TODO: Destroy buffers

        gfx::destroy_pipeline(m_pipeline);
        m_pipeline = {};

        // #TODO: Destroy descriptor set

        gfx::destroy_texture(m_fontTexture);
        m_fontTexture = {};
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

    void SystemImGui::render_ui(gfx::CommandListHandle cmdList, u32 frameIndex)
    {
        RUNE_UNUSED(frameIndex);

        ImGui::Render();
        auto* drawData = ImGui::GetDrawData();

        auto fbWidth = drawData->DisplaySize.x * drawData->FramebufferScale.x;
        auto fbHeight = drawData->DisplaySize.y * drawData->FramebufferScale.y;
        if (fbWidth <= 0 || fbHeight <= 0)
        {
            return;
        }

        auto& rb = m_renderBuffers.at(frameIndex);

        if (drawData->TotalVtxCount > 0)
        {
            // Create or Resize vertex/index buffers
            sizet vertexSize = drawData->TotalVtxCount * sizeof(ImDrawVert);
            sizet indexSize = drawData->TotalIdxCount * sizeof(ImDrawIdx);
            if (!rb.vtxBuffer || rb.vertexBufferSize < vertexSize)
            {
                create_or_resize_buffer(rb.vtxBuffer, rb.vertexBufferSize, vertexSize, gfx::BufferType::eVertex);
            }
            if (!rb.idxBuffer || rb.indexBufferSize < indexSize)
            {
                create_or_resize_buffer(rb.idxBuffer, rb.indexBufferSize, indexSize, gfx::BufferType::eIndex);
            }

            ImDrawVert* vtxDst{ nullptr };
            ImDrawIdx* idxDst{ nullptr };
            gfx::map_buffer(rb.vtxBuffer, (void*&)vtxDst);
            gfx::map_buffer(rb.idxBuffer, (void*&)idxDst);
            for (auto i = 0; i < drawData->CmdListsCount; ++i)
            {
                const auto* imguiCmdList = drawData->CmdLists[i];
                std::memcpy(vtxDst, imguiCmdList->VtxBuffer.Data, imguiCmdList->VtxBuffer.Size * sizeof(ImDrawVert));
                std::memcpy(idxDst, imguiCmdList->IdxBuffer.Data, imguiCmdList->IdxBuffer.Size * sizeof(ImDrawIdx));
                vtxDst += imguiCmdList->VtxBuffer.Size;
                idxDst += imguiCmdList->IdxBuffer.Size;
            }

            gfx::unmap_buffer(rb.vtxBuffer);
            gfx::unmap_buffer(rb.idxBuffer);
        }

        // Setup render state
        gfx::bind_pipeline(cmdList, m_pipeline);

        if (drawData->TotalVtxCount > 0)
        {
            gfx::bind_vertex_buffers(cmdList, 0, { rb.vtxBuffer });
            gfx::bind_index_buffer(cmdList, rb.idxBuffer, gfx::IndexType::eUInt16);
        }

        // Unflip viewport for Vulkan
        gfx::set_viewport(cmdList,
                          drawData->DisplayPos.x,
                          drawData->DisplayPos.y + drawData->DisplaySize.y,
                          drawData->DisplayPos.x + drawData->DisplaySize.x,
                          -drawData->DisplaySize.y);

        // Setup scale & translation
        {
            glm::vec2 scale{
                2.0f / drawData->DisplaySize.x,
                2.0f / drawData->DisplaySize.y,
            };
            glm::vec2 translate{
                -1.0f - drawData->DisplayPos.x * scale.x,
                -1.0f - drawData->DisplayPos.y * scale.y,
            };
            gfx::set_constants(cmdList, gfx::ShaderStageFlags_Vertex, sizeof(glm::vec2) * 0, sizeof(glm::vec2), &scale.x);
            gfx::set_constants(cmdList, gfx::ShaderStageFlags_Vertex, sizeof(glm::vec2) * 1, sizeof(glm::vec2), &translate.x);
        }

        auto clipOff = drawData->DisplayPos;

        // Maintain offsets because we merged all buffers into one
        u32 globalVtxOffset{ 0 };
        u32 globalIdxOffset{ 0 };
        for (auto n = 0; n < drawData->CmdListsCount; ++n)
        {
            const auto* imguiCmdList = drawData->CmdLists[n];
            for (auto imguiCmdIdx = 0; imguiCmdIdx < imguiCmdList->CmdBuffer.Size; ++imguiCmdIdx)
            {
                const auto* pcmd = &imguiCmdList->CmdBuffer[imguiCmdIdx];
                if (pcmd->UserCallback)
                {
                    pcmd->UserCallback(imguiCmdList, pcmd);
                }
                else
                {
                    // Project scissor/clipping rectangles into framebuffer space
                    auto clipMin = glm::vec2(pcmd->ClipRect.x - clipOff.x, pcmd->ClipRect.y - clipOff.y);
                    auto clipMax = glm::vec2(pcmd->ClipRect.z - clipOff.x, pcmd->ClipRect.w - clipOff.y);
                    // Clamp viewport bounds
                    clipMin = glm::max(clipMin, { 0, 0 });
                    clipMax = glm::min(clipMax, { fbWidth, fbHeight });
                    if (clipMax.x <= clipMin.x || clipMax.y <= clipMin.y)
                    {
                        continue;
                    }

                    // Apply scissor/clipping rect
                    gfx::set_scissor(cmdList, i32(clipMin.x), i32(clipMin.y), u32(clipMax.x - clipMin.x), u32(clipMax.y - clipMin.y));

                    auto setHandle = *static_cast<gfx::DescriptorSetHandle*>(pcmd->GetTexID());
                    gfx::bind_descriptor_sets(cmdList, 0, { setHandle });

                    gfx::draw_indexed(
                        cmdList, pcmd->ElemCount, 1, pcmd->IdxOffset + globalIdxOffset, i32(pcmd->VtxOffset + globalVtxOffset), 0);
                }
            }
            globalVtxOffset += imguiCmdList->VtxBuffer.Size;
            globalIdxOffset += imguiCmdList->IdxBuffer.Size;
        }
    }
}