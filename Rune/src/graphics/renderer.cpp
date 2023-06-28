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
            .descriptorSets = {},
        };
        if (!gfx::create_graphics_pipeline(m_pipeline, device, pipelineInfo))
        {
            RUNE_THROW_EX("Failed to create GFX graphics pipeline!");
        }
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
    }

    void Renderer::flush_camera(const RenderCamera& camera)
    {
        RUNE_ASSERT(m_swapchainMap.contains(camera.targetWindow));

        auto swapchainHandle = m_swapchainMap.at(camera.targetWindow);
        auto& frameData = m_frames.at(m_frameIndex);

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
            gfx::set_viewport(cmdList, 0, 0, 800, 600);
            gfx::set_scissor(cmdList, 0, 0, 800, 600);

            gfx::bind_pipeline(cmdList, m_pipeline);
            gfx::draw(cmdList, 3, 1, 0, 0);
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
}