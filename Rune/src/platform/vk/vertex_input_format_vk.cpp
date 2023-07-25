#include "vertex_input_format_vk.hpp"

#include "type_conversions_vk.hpp"

namespace rune::rhi
{
    auto VertexInputFormat::create(const VertexInputFormatDecl& decl) -> Owned<VertexInputFormat>
    {
        return create_owned<VertexInputFormatVulkan>(decl);
    }

    VertexInputFormatVulkan::VertexInputFormatVulkan(const VertexInputFormatDecl& decl)
    {
        m_attributes.reserve(decl.Attributes.size());
        for (const auto& attribute : decl.Attributes)
        {
            auto& vkAttribute = m_attributes.emplace_back();
            vkAttribute.setBinding(attribute.Binding);
            vkAttribute.setLocation(attribute.Location);
            vkAttribute.setFormat(convert(attribute.Format));
            vkAttribute.setOffset(attribute.Offset);
        }

        m_bindings.reserve(decl.Bindings.size());
        for (const auto& binding : decl.Bindings)
        {
            auto& vkBinding = m_bindings.emplace_back();
            vkBinding.setBinding(binding.Binding);
            vkBinding.setStride(binding.Stride);
            // vkBinding.setInputRate(); // #TODO: InputRate
        }

        m_pipelineVertexInputState.setVertexAttributeDescriptions(m_attributes);
        m_pipelineVertexInputState.setVertexBindingDescriptions(m_bindings);
    }

}