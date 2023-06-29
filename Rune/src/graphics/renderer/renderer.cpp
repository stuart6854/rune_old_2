#include "graphics/renderer/renderer.hpp"

#include "internal_common.hpp"
#include "graphics/graphics.hpp"
#include "platform/platform.hpp"
#include "io/io.hpp"

#include "gfx/gfx.hpp"

#include <array>
#include <memory>
#include <vector>
#include <unordered_map>

using namespace sm;

namespace rune::graphics::renderer
{
    struct RendererData
    {
        std::unordered_map<platform::WindowHandle, gfx::SwapChainHandle> swapchainMap{};

        std::vector<RenderCamera> camerasToRender{};
        std::vector<DrawCall> drawCalls{};

        std::vector<glm::mat4> instances{};
        std::vector<std::shared_ptr<StaticMesh>> meshes{};  // TODO: Make Static/Skeletal agnostic

        gfx::BufferHandle cameraBuffer{};
        gfx::PipelineHandle pipeline{};
        gfx::DescriptorSetHandle set{};

        struct FrameData
        {
            std::vector<gfx::FenceHandle> fences{};
            std::vector<gfx::SemaphoreHandle> semaphores{};
            std::vector<gfx::CommandListHandle> commandLists{};
        };
        std::array<FrameData, FRAME_BUFFER_COUNT> frames{};
        std::uint32_t frameIndex{};
    };
    std::unique_ptr<RendererData> g_rendererData{ nullptr };  // NOLINT

    void flush_camera(const RenderCamera& camera);
    void geometry_pass(gfx::CommandListHandle cmdList);

    void initialise()
    {
        RUNE_ASSERT(g_rendererData == nullptr);
        g_rendererData = std::make_unique<RendererData>();
        auto& rendererData = *g_rendererData;

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
            .depthTest = false,
        };
        if (!gfx::create_graphics_pipeline(rendererData.pipeline, device, pipelineInfo))
        {
            RUNE_THROW_EX("Failed to create GFX graphics pipeline!");
        }

        gfx::create_buffer(rendererData.cameraBuffer, device, { gfx::BufferType::eUniform, sizeof(glm::mat4) * 2 });

        gfx::create_descriptor_set_from_pipeline(rendererData.set, rendererData.pipeline, 0);
        gfx::bind_buffer_to_descriptor_set(rendererData.set, 0, rendererData.cameraBuffer);
    }

    void shutdown()
    {
        RUNE_ASSERT(g_rendererData != nullptr);

        gfx::wait_for_device_idle(graphics::get_device());
    }

    void render_camera(const RenderCamera& camera)
    {
        RUNE_ASSERT(g_rendererData != nullptr);
        auto& rendererData = *g_rendererData;

        if (!rendererData.swapchainMap.contains(camera.targetWindow))
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

            rendererData.swapchainMap.insert({ camera.targetWindow, swapChainHandle }).first;
        }

        rendererData.camerasToRender.emplace_back(camera);
    }

    void render_static_mesh(const std::shared_ptr<StaticMesh>& mesh, const glm::mat4& transform)
    {
        RUNE_ASSERT(g_rendererData != nullptr);
        auto& rendererData = *g_rendererData;

        rendererData.instances.push_back(transform);
        auto instanceIdx = static_cast<u32>(rendererData.instances.size() - 1);

        rendererData.meshes.push_back(mesh);
        auto meshIdx = static_cast<u32>(rendererData.meshes.size() - 1);

        auto& drawCall = rendererData.drawCalls.emplace_back();
        drawCall.instance = instanceIdx;
        drawCall.mesh = meshIdx;
    }

    void flush_renders()
    {
        RUNE_ASSERT(g_rendererData != nullptr);
        auto& rendererData = *g_rendererData;

        // #TODO: Setup global uniform data

        rendererData.frameIndex = (rendererData.frameIndex + 1) % FRAME_BUFFER_COUNT;
        auto& frameData = rendererData.frames.at(rendererData.frameIndex);
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

        for (const auto& camera : rendererData.camerasToRender)
        {
            flush_camera(camera);
        }
        rendererData.camerasToRender.clear();
        rendererData.drawCalls.clear();
        rendererData.instances.clear();
    }

    auto create_static_mesh() -> std::shared_ptr<StaticMesh>
    {
        return std::make_shared<StaticMesh>();
    }

    void flush_camera(const RenderCamera& camera)
    {
        auto& rendererData = *g_rendererData;

        RUNE_ASSERT(rendererData.swapchainMap.contains(camera.targetWindow));

        auto swapchainHandle = rendererData.swapchainMap.at(camera.targetWindow);
        auto& frameData = rendererData.frames.at(rendererData.frameIndex);

        std::array<glm::mat4, 2> cameraData = { camera.projMatrix, camera.viewMatrix };

        void* dataPtr{ nullptr };
        gfx::map_buffer(rendererData.cameraBuffer, dataPtr);
        std::memcpy(dataPtr, cameraData.data(), sizeof(cameraData));
        gfx::unmap_buffer(rendererData.cameraBuffer);

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

    void geometry_pass(gfx::CommandListHandle cmdList)
    {
        auto& rendererData = *g_rendererData;

        gfx::bind_pipeline(cmdList, rendererData.pipeline);
        gfx::bind_descriptor_set(cmdList, rendererData.set);
        for (const auto& drawCall : rendererData.drawCalls)
        {
            const auto& instance = rendererData.instances.at(drawCall.instance);
            gfx::set_constants(cmdList, gfx::ShaderStageFlags_Vertex, 0, sizeof(instance), &instance);
            gfx::draw(cmdList, 3, 1, 0, 0);
        }
    }

}