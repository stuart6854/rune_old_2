#include "graphics/renderer.hpp"

#include "graphics/graphics.hpp"
#include "platform/platform.hpp"
#include "io/io.hpp"

namespace rune::graphics
{
    void Renderer::initialise()
    {
        auto device = graphics::get_device();

        const auto vertShaderBinary = io::read_binary<std::uint32_t>("triangle.vert.spv").value();
        const auto fragShaderBinary = io::read_binary<std::uint32_t>("triangle.frag.spv").value();
        gfx::GraphicsPipelineInfo pipelineInfo{
            .vertexCode = vertShaderBinary,
            .fragmentCode = fragShaderBinary,
            .descriptorSets = { {
                .bindings = {
                    { gfx::DescriptorType::eUniformBuffer, 1, gfx::ShaderStageFlags_Vertex },
                },
            } },
            .constantBlock = { sizeof(glm::mat4), gfx::ShaderStageFlags_Vertex },
        };
        if (!gfx::create_graphics_pipeline(m_pipeline, device, pipelineInfo))
        {
            RUNE_THROW_EX("Failed to create GFX graphics pipeline!");
        }

        gfx::create_buffer(m_cameraBuffer, device, { gfx::BufferType::eUniform, sizeof(glm::mat4) * 2 });

        gfx::create_descriptor_set_from_pipeline(m_set, m_pipeline, 0);
        gfx::bind_buffer_to_descriptor_set(m_set, 0, m_cameraBuffer);
    }

    void Renderer::shutdown()
    {
        gfx::wait_for_device_idle(graphics::get_device());
    }

    void Renderer::render_camera(const RenderCamera& camera)
    {
        if (!m_swapchainMap.contains(camera.targetWindow))
        {
            auto windowSize = platform::get_window_size_pixels(camera.targetWindow);
            gfx::SwapChainInfo swapChainInfo{
                .platformWindowHandle = platform::get_window_platform_handle(camera.targetWindow),
                .initialWidth = windowSize.x,
                .initialHeight = windowSize.y,
            };
            gfx::SwapChainHandle swapChainHandle{};
            if (!gfx::create_swap_chain(swapChainHandle, graphics::get_device(), swapChainInfo))
            {
                LOG_ERROR("graphics - renderer - Failed to create window swapchain!");
                return;
            }

            m_swapchainMap.insert({ camera.targetWindow, swapChainHandle }).first;
        }

        m_camerasToRender.emplace_back(camera);
    }

    void Renderer::render_static_mesh(const glm::mat4& transform)
    {
        m_instances.push_back(transform);
        auto instance = static_cast<u32>(m_instances.size() - 1);

        auto& drawCall = m_drawCalls.emplace_back();
        drawCall.instance = instance;
    }

    void Renderer::flush_renders()
    {
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
            gfx::destroy_command_list(graphics::get_device(), cmdList);
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

    void Renderer::flush_camera(const RenderCamera& camera)
    {
        RUNE_ASSERT(m_swapchainMap.contains(camera.targetWindow));

        auto swapchainHandle = m_swapchainMap.at(camera.targetWindow);
        auto& frameData = m_frames.at(m_frameIndex);

        glm::mat4 cameraData[] = { camera.projMatrix, camera.viewMatrix };

        void* dataPtr{ nullptr };
        gfx::map_buffer(m_cameraBuffer, dataPtr);
        std::memcpy(dataPtr, &cameraData[0], sizeof(cameraData));
        gfx::unmap_buffer(m_cameraBuffer);

        gfx::CommandListHandle cmdList{};
        if (!gfx::create_command_list(cmdList, graphics::get_device(), 0))
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
            .depthAttachment = {},
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

        gfx::present_swap_chain(swapchainHandle, 0, &semaphoreHandle);
    }

    void Renderer::geometry_pass(gfx::CommandListHandle cmdList)
    {
        gfx::bind_pipeline(cmdList, m_pipeline);
        gfx::bind_descriptor_set(cmdList, m_set);
        for (const auto& drawCall : m_drawCalls)
        {
            const auto& instance = m_instances.at(drawCall.instance);
            gfx::set_constants(cmdList, gfx::ShaderStageFlags_Vertex, 0, sizeof(instance), &instance);
            gfx::draw(cmdList, 3, 1, 0, 0);
        }
    }

}