#include "graphics/graphics.hpp"

#include "internal_common.hpp"
#include "io/io.hpp"

namespace rune::graphics
{
    namespace
    {
        gfx::DeviceHandle g_device{};        // NOLINT
        gfx::SwapChainHandle g_swapchain{};  // NOLINT
        gfx::PipelineHandle g_pipeline{};    // NOLINT
        gfx::CommandListHandle g_cmd{};      // NOLINT
    }

    void initialise(void* platformWindowHandle)
    {
        gfx::set_error_callback([](const char* msg) { LOG_ERROR("{}", msg); });

        gfx::AppInfo appInfo{
            .appName = "rune_sandbox",
            .engineName = "Rune",
        };
        if (!gfx::initialise(appInfo))
        {
            throw std::runtime_error("graphics - Failed to initialise graphics layer!");
        }

        gfx::DeviceInfo deviceInfo{
            .deviceFlags = gfx::DeviceFlags_PreferDiscrete,
            .queueFlags = { gfx::QueueFlags_Graphics },
        };

        if (!gfx::create_device(g_device, deviceInfo))
        {
            throw std::runtime_error("graphics - Failed to create device!");
        }

        gfx::SwapChainInfo swapChainInfo{
            .platformWindowHandle = platformWindowHandle,
            .initialWidth = 800,
            .initialHeight = 600,
        };
        if (!gfx::create_swap_chain(g_swapchain, g_device, swapChainInfo))
        {
            throw std::runtime_error("graphics - Failed to create swap chain!");
        }

        const auto vertShaderBinary = io::read_binary<std::uint32_t>("triangle.vert.spv").value();
        const auto fragShaderBinary = io::read_binary<std::uint32_t>("triangle.frag.spv").value();
        gfx::GraphicsPipelineInfo pipelineInfo{
            .vertexCode = vertShaderBinary,
            .fragmentCode = fragShaderBinary,
            .descriptorSets = {},
        };
        if (!gfx::create_graphics_pipeline(g_pipeline, g_device, pipelineInfo))
        {
            throw std::runtime_error("Failed to create GFX graphics pipeline!");
        }

        if (!gfx::create_command_list(g_cmd, g_device, 0))
        {
            throw std::runtime_error("Failed to create GFX command list!");
        }
    }

    void shutdown()
    {
        gfx::destroy_device(g_device);
        gfx::shutdown();
    }

    void render_temp()
    {
        gfx::reset(g_cmd);
        gfx::begin(g_cmd);

        gfx::TextureHandle swapChainImageHandle{};
        if (!gfx::get_swap_chain_image(swapChainImageHandle, g_swapchain))
        {
            throw std::runtime_error("Failed to get SwapChain image handle!");
        }

        gfx::transition_texture(g_cmd, swapChainImageHandle, gfx::TextureState::eUndefined, gfx::TextureState::eRenderTarget);

        gfx::RenderPassInfo renderPassInfo{
            .colorAttachments = { swapChainImageHandle },
            .depthAttachment = {},
            .clearColor = { 0.392f, 0.584f, 0.929f, 1.0f },  // Cornflower Blue
        };
        gfx::begin_render_pass(g_cmd, renderPassInfo);
        {
            gfx::set_viewport(g_cmd, 0, 0, 800, 600);
            gfx::set_scissor(g_cmd, 0, 0, 800, 600);

            gfx::bind_pipeline(g_cmd, g_pipeline);
            gfx::draw(g_cmd, 3, 1, 0, 0);
        }
        gfx::end_render_pass(g_cmd);

        gfx::transition_texture(g_cmd, swapChainImageHandle, gfx::TextureState::eRenderTarget, gfx::TextureState::ePresent);

        gfx::end(g_cmd);

        gfx::SubmitInfo submitInfo{ .commandList = g_cmd, .waitSemaphoreHandle = {} };
        gfx::FenceHandle fenceHandle{};
        gfx::submit_command_list(submitInfo, &fenceHandle, nullptr);

        gfx::wait_on_fence(fenceHandle);

        gfx::present_swap_chain(g_swapchain, 0, nullptr);
    }

}