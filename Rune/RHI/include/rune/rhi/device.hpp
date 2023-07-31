#pragma once

#include "structs.hpp"
#include "resources.hpp"

#include <any>
#include <vector>

namespace rune::rhi
{
    class Device
    {
    public:
        Device(bool useDebugLayer);
        ~Device();

        /* Create a swapchain. If recreating a swapchain, window can be nullptr. */
        bool create_swapchain(const SwapChainDesc& desc, void* window, Swapchain& swapchain);
        bool create_buffer(const BufferDesc& desc, Buffer& buffer);

        bool create_shader_program(const ShaderProgramDesc& desc, ShaderProgram& program);
        bool create_shader_resource_binding(/* Shader, ShaderResourceBinding& shaderResourceBinding */);

        /**
         * @brief Begins a new command list for GPU recording.
         * The command list remains valid until submit_command_lists() is called.
         */
        auto begin_command_list(QueueType queueType = QueueType::Graphics) -> CommandList;

        /**
         * @brief Submits all begun command lists. Also presents all SwapChains.
         */
        void submit_command_lists();

        /**
         * @brief Blocks the CPU until all submitted GPU work is completed.
         */
        void wait_for_gpu();

        /**
         * @brief Hand resource over to device to handle deletion of resource safely.
         * @param resource
         */
        void defer_deletion(Buffer& resource);

        void begin_render_pass(Swapchain& swapchain, CommandList& cmdList);
        void begin_render_pass(const std::vector<RenderPassImage>& images, CommandList& cmdList);
        void end_render_pass(CommandList& cmdList);
        void set_pipeline_state(PipelineState& state, CommandList& cmdList);

        auto get_internal_state() -> std::any& { return m_internalState; }

    private:
        std::any m_internalState;
    };

}