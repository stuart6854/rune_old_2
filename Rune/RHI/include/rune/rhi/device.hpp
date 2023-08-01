#pragma once

#include "structs.hpp"
#include "resources.hpp"

#include <any>
#include <vector>

namespace rune::rhi
{
    struct DeviceInternal;
    struct Device
    {
        std::shared_ptr<DeviceInternal> internal;

        Device(bool useDebugLayer);
        ~Device();

        bool create_command_list(QueueType queueType, CommandList& cmdList);
        bool create_fence(bool signaled, Fence& fence);
        /* Create a swapchain. If recreating a swapchain, window can be nullptr. */
        bool create_swapchain(const SwapChainDesc& desc, void* window, Swapchain& swapchain);
        bool create_buffer(const BufferDesc& desc, Buffer& buffer);

        bool create_shader_program(ShaderProgramDesc& desc, ShaderProgram& program);
        bool create_shader_resource_binding(/* Shader, ShaderResourceBinding& shaderResourceBinding */);

        /**
         * @brief Submits command lists. Also presents all used Swapchains.
         */
        void submit_command_lists(const std::vector<CommandList>& cmdList, Fence& fence);

        void wait_for_fence(Fence& fence);

        /**
         * @brief Blocks the CPU until all submitted GPU work is completed.
         */
        void wait_for_gpu();

        /**
         * @brief Hand resource over to device to handle deletion of resource safely.
         * @param resource
         */
        void destroy_resource(Buffer& resource);

        void begin(CommandList& cmdList);
        void end(CommandList& cmdList);
        void begin_render_pass(Swapchain& swapchain, CommandList& cmdList);
        void begin_render_pass(const std::vector<RenderPassImage>& images, CommandList& cmdList);
        void end_render_pass(CommandList& cmdList);
        void set_pipeline_state(PipelineState& state, CommandList& cmdList);
        void set_viewport(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth, CommandList& cmdList);
        void set_scissor(std::int32_t left, std::int32_t top, std::int32_t right, std::int32_t bottom, CommandList& cmdList);
        void draw(std::uint32_t vertexCount, std::uint32_t vertexOffset, CommandList& cmdList);
        void draw_indexed(std::uint32_t indexCount, std::uint32_t indexOffset, std::uint32_t vertexOffset, CommandList& cmdList);
    };

}