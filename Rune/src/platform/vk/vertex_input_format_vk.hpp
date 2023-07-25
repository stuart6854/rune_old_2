#pragma once

#include "rune/rendering/rhi/vertex_input_format.hpp"

#include <vulkan/vulkan.hpp>

namespace rune::rhi
{
    class VertexInputFormatVulkan : public VertexInputFormat
    {
    public:
        VertexInputFormatVulkan(const VertexInputFormatDecl& decl);
        ~VertexInputFormatVulkan() = default;

        auto get_vk_pipeline_vertex_input_state() const -> const auto& { return m_pipelineVertexInputState; }

    private:
        std::vector<vk::VertexInputAttributeDescription> m_attributes;
        std::vector<vk::VertexInputBindingDescription> m_bindings;

        vk::PipelineVertexInputStateCreateInfo m_pipelineVertexInputState{};
    };
}