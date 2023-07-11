#include "rendering/system_renderer.hpp"

#include "common_internal.hpp"
#include "rendering/static_mesh.hpp"
#include "rendering/texture.hpp"
#include "rendering/material.hpp"
#include "platform/system_platform.hpp"
#include "io/io.hpp"

#include <array>
#include <memory>
#include <vector>
#include <cstring>

using namespace sm;

namespace rune
{
    void SystemRenderer::initialise()
    {
        gfx::set_error_callback([](const char* msg) { LOG_ERROR("{}", msg); });

        gfx::AppInfo appInfo{
            .appName = "rune_sandbox",
            .engineName = "Rune",
        };
        if (!gfx::initialise(appInfo))
        {
            RUNE_THROW_EX("graphics - Failed to initialise graphics layer!");
        }

        gfx::DeviceInfo deviceInfo{
            .deviceFlags = gfx::DeviceFlags_PreferDiscrete,
            .queueFlags = { gfx::QueueFlags_Graphics },
        };

        if (!gfx::create_device(m_device, deviceInfo))
        {
            RUNE_THROW_EX("graphics - Failed to create device!");
        }

        Engine::get().get_system<SystemEvents>()->register_receiver([this](auto&& msg)
                                                                    { on_event_recieved(std::forward<decltype(msg)>(msg)); });

        LOG_INFO("Renderer system initialised.");

        gfx::TextureInfo depthAttachmentInfo{
            .usage = gfx::TextureUsage::eDepthStencilAttachment,
            .type = gfx::TextureType::e2D,
            .width = 1280,
            .height = 720,
            .format = gfx::Format::eDepth24Stencil8,
            .mipLevels = 1,
        };
        if (!gfx::create_texture(m_depthAttachment, m_device, depthAttachmentInfo))
        {
            RUNE_THROW_EX("Failed to create GFX depth attachment texture!");
        }

        const auto vertShaderBinary = io::read_binary<std::uint32_t>("triangle.vert.spv").value();
        const auto fragShaderBinary = io::read_binary<std::uint32_t>("triangle.frag.spv").value();
        gfx::GraphicsPipelineInfo pipelineInfo{
            .vertexCode = vertShaderBinary,
            .fragmentCode = fragShaderBinary,
            .vertexInputBindings = {
                gfx::VertexBinding("Positions", { gfx::VertexAttribute("Position", 0, gfx::Format::eRGB32), }),
                gfx::VertexBinding("NormalsTexCoords", {
                                                           gfx::VertexAttribute("Normals", 1, gfx::Format::eRGB32),
                                                           gfx::VertexAttribute("TexCoords", 2, gfx::Format::eRG32),
                                                       }),
            },
            .descriptorSets = {
                {
                    .bindings = {
                        { gfx::DescriptorType::eUniformBuffer, 1, gfx::ShaderStageFlags_Vertex },
                    },
                },
                {
                    .bindings = {
                        { gfx::DescriptorType::eTexture, 1, gfx::ShaderStageFlags_Fragment },
                    },
                },
            },
            .constantBlock = { sizeof(glm::mat4), gfx::ShaderStageFlags_Vertex },
            .depthTest = true,
            .colorAttachments = { gfx::Format::eBGRA8 },
            .depthAttachmentFormat = gfx::Format::eDepth24Stencil8,
        };
        if (!gfx::create_graphics_pipeline(m_pipeline, m_device, pipelineInfo))
        {
            RUNE_THROW_EX("Failed to create GFX graphics pipeline!");
        }

        gfx::create_buffer(m_cameraBuffer, m_device, { gfx::BufferType::eUniform, sizeof(glm::mat4) * 2 });

        gfx::create_descriptor_set_from_pipeline(m_set, m_pipeline, 0);
        gfx::bind_buffer_to_descriptor_set(m_set, 0, m_cameraBuffer);
    }

    void SystemRenderer::update() {}

    void SystemRenderer::shutdown()
    {
        RUNE_PROFILE_SCOPE();

        gfx::wait_for_device_idle(m_device);
        gfx::destroy_device(m_device);
        gfx::shutdown();

        LOG_INFO("Renderer system has shut down.");
    }

    void SystemRenderer::render_camera(const RenderCamera& camera)
    {
        RUNE_PROFILE_SCOPE();

        if (!m_swapchainMap.contains(camera.targetWindow))
        {
            auto* platformSystem = Engine::get().get_system<SystemPlatform>();
            auto windowSize = platformSystem->get_window_size_pixels(camera.targetWindow);
            gfx::SwapChainInfo swapChainInfo{
                .platformDisplayHandle = platformSystem->get_platform_display_handle(),
                .platformWindowHandle = platformSystem->get_window_platform_handle(camera.targetWindow),
                .initialWidth = windowSize.x,
                .initialHeight = windowSize.y,
            };
            gfx::SwapChainHandle swapChainHandle{};
            if (!gfx::create_swap_chain(swapChainHandle, m_device, swapChainInfo))
            {
                LOG_ERROR("graphics - renderer - Failed to create window swapchain!");
                return;
            }

            m_swapchainMap.insert({ camera.targetWindow, swapChainHandle });
        }

        m_camerasToRender.emplace_back(camera);
    }

    void SystemRenderer::render_static_mesh(const StaticMesh* mesh, const std::vector<Material*>& materials, const glm::mat4& transform)
    {
        RUNE_PROFILE_SCOPE();

        if (mesh == nullptr)
        {
            return;
        }

        m_instances.push_back(transform);
        auto instanceIdx = static_cast<u32>(m_instances.size() - 1);

        auto& renderMesh = m_meshes.emplace_back();
        renderMesh.indexCount = mesh->get_index_count();
        renderMesh.indexBuffer = mesh->get_index_buffer();
        renderMesh.vertexBuffers = mesh->get_vertex_buffers();
        auto meshIdx = static_cast<u32>(m_meshes.size() - 1);

        m_materials.push_back(materials[0]);
        auto materialId = static_cast<u32>(m_materials.size() - 1);

        auto& drawCall = m_drawCalls.emplace_back();
        drawCall.instance = instanceIdx;
        drawCall.mesh = meshIdx;
        drawCall.material = materialId;
    }

    void SystemRenderer::flush()
    {
        RUNE_PROFILE_SCOPE();

        // #TODO: Setup global uniform data

        m_frameIndex = (m_frameIndex + 1) % FRAME_BUFFER_COUNT;
        auto& frameData = m_frames.at(m_frameIndex);
        for (auto fence : frameData.fences)
        {
            gfx::wait_on_fence(fence);
        }
        frameData.fences.clear();
        for (auto semaphore : frameData.semaphores)
        {
            gfx::destroy_semaphore(semaphore);
        }
        frameData.semaphores.clear();
        for (auto cmdList : frameData.commandLists)
        {
            gfx::destroy_command_list(m_device, cmdList);
        }
        frameData.commandLists.clear();

        for (const auto& camera : m_camerasToRender)
        {
            flush_camera(camera);
        }
        m_camerasToRender.clear();
        m_drawCalls.clear();
        m_instances.clear();
    }

    auto SystemRenderer::create_static_mesh() -> std::unique_ptr<StaticMesh>
    {
        return std::make_unique<StaticMesh>(*this);
    }

    auto SystemRenderer::create_texture() -> std::unique_ptr<Texture>
    {
        return std::make_unique<Texture>(*this);
    }

    auto SystemRenderer::create_material() -> std::unique_ptr<Material>
    {
        return std::make_unique<Material>(*this);
    }

    void SystemRenderer::on_event_recieved(const Event& msg)
    {
        LOG_INFO("Event: {}", u64(msg.type));
        // #TODO: Recreate render resources (attachments, etc.)
    }

    void SystemRenderer::flush_camera(const RenderCamera& camera)
    {
        RUNE_PROFILE_SCOPE();

        RUNE_ASSERT(m_swapchainMap.contains(camera.targetWindow));

        auto swapchainHandle = m_swapchainMap.at(camera.targetWindow);
        auto& frameData = m_frames.at(m_frameIndex);

        std::array<glm::mat4, 2> cameraData = { camera.projMatrix, camera.viewMatrix };

        void* dataPtr{ nullptr };
        gfx::map_buffer(m_cameraBuffer, dataPtr);
        std::memcpy(dataPtr, cameraData.data(), sizeof(cameraData));
        gfx::unmap_buffer(m_cameraBuffer);

        gfx::CommandListHandle cmdList{};
        if (!gfx::create_command_list(cmdList, m_device, 0))
        {
            RUNE_THROW_EX("graphics - renderer - Failed to create command list!");
        }

        // #TODO: Setup camera uniform data

        gfx::begin(cmdList);
        gfx::TextureHandle swapChainImageHandle{};
        if (!gfx::get_swap_chain_image(swapChainImageHandle, swapchainHandle))
        {
            RUNE_THROW_EX("Failed to get SwapChain image handle!");
        }

        gfx::transition_texture(cmdList, swapChainImageHandle, gfx::TextureState::eUndefined, gfx::TextureState::eRenderTarget);

        gfx::RenderPassInfo renderPassInfo{
            .colorAttachments = { swapChainImageHandle },
            .depthAttachment = { m_depthAttachment },
            .clearColor = { 0.392f, 0.584f, 0.929f, 1.0f },  // Cornflower Blue
        };
        gfx::begin_render_pass(cmdList, renderPassInfo);
        {
            gfx::set_viewport(cmdList, 0, 0, f32(camera.targetWindowSize.x), f32(camera.targetWindowSize.y));
            gfx::set_scissor(cmdList, 0, 0, camera.targetWindowSize.x, camera.targetWindowSize.y);

            geometry_pass(cmdList);
        }
        gfx::end_render_pass(cmdList);

        gfx::transition_texture(cmdList, swapChainImageHandle, gfx::TextureState::eRenderTarget, gfx::TextureState::ePresent);

        gfx::end(cmdList);

        gfx::SubmitInfo submitInfo{ .commandList = cmdList, .waitSemaphoreHandle = {} };
        gfx::FenceHandle fenceHandle{};
        gfx::SemaphoreHandle semaphoreHandle{};
        gfx::submit_command_list(submitInfo, &fenceHandle, &semaphoreHandle);

        frameData.fences.push_back(fenceHandle);
        frameData.semaphores.push_back(semaphoreHandle);
        frameData.commandLists.push_back(cmdList);

        {
            RUNE_PROFILE_SECTION("Present");
            gfx::present_swap_chain(swapchainHandle, 0, &semaphoreHandle);
        }
    }

    void SystemRenderer::geometry_pass(gfx::CommandListHandle cmdList)
    {
        RUNE_PROFILE_SCOPE();

        gfx::bind_pipeline(cmdList, m_pipeline);
        gfx::bind_descriptor_sets(cmdList, 0, { m_set });
        for (const auto& drawCall : m_drawCalls)
        {
            const auto* material = m_materials.at(drawCall.material);
            gfx::bind_descriptor_sets(cmdList, 1, { material->get_texture_set() });

            const auto& renderMesh = m_meshes.at(drawCall.mesh);
            gfx::bind_index_buffer(cmdList, renderMesh.indexBuffer, gfx::IndexType::eUInt32);
            gfx::bind_vertex_buffers(cmdList, 0, renderMesh.vertexBuffers);

            const auto& instance = m_instances.at(drawCall.instance);
            gfx::set_constants(cmdList, gfx::ShaderStageFlags_Vertex, 0, sizeof(instance), &instance);
            gfx::draw_indexed(cmdList, renderMesh.indexCount, 1, 0, 0, 0);
        }
    }

}