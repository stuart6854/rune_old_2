#pragma once

#include "rune/rendering/rhi/device.hpp"

#include <vulkan/vulkan.hpp>
#include <vulkan-memory-allocator-hpp/vk_mem_alloc.hpp>

namespace rune::rhi
{
    class InstanceVulkan;
    class SurfaceVulkan;
    class Fence;

    class DeviceVulkan : public Device
    {
    public:
        bool initialise();
        bool destroy();

        auto get_vk_instance() const -> vk::Instance;
        auto get_vk_physical_device() const -> vk::PhysicalDevice { return m_physicalDevice; }
        auto get_vk_device() const -> vk::Device { return m_device; }
        auto get_vk_allocator() const -> vma::Allocator { return m_allocator; }
        auto get_vk_graphics_queue() const -> vk::Queue { return m_graphicsQueue; }
        auto get_vk_cmd_pool() const -> vk::CommandPool { return m_cmdPool; }

        auto get_or_create_set_layout(vk::DescriptorSetLayoutCreateInfo& createInfo) -> vk::DescriptorSetLayout;
        auto get_or_create_pipeline_layout(vk::PipelineLayoutCreateInfo& createInfo) -> vk::PipelineLayout;

        auto create_surface(const SurfaceDecl& decl) -> Owned<Surface> override;
        auto create_cmd_list(bool autoSubmit = true) -> Owned<CommandList> override;

        void submit(Fence* fence, u64 fenceValue) override;

        void submit_single(CommandList& cmdList, Fence* fence, u64 fenceValue) override;

    protected:
        friend class CommandListVulkan;

        void on_cmd_list_reset(CommandList& cmdList) override;
        void on_cmd_list_begin(CommandList& cmdList) override;

    private:
        Shared<InstanceVulkan> m_instance{};
        vk::PhysicalDevice m_physicalDevice{};
        vk::Device m_device{};
        vma::Allocator m_allocator{};

        vk::Queue m_graphicsQueue{};

        vk::CommandPool m_cmdPool{};

        std::vector<SurfaceVulkan*> m_surfaces{};

        std::vector<vk::CommandBuffer> m_cmdBufferSubmissionOrder{};

        std::unordered_map<hasht, vk::DescriptorSetLayout> m_setLayoutMap{};
        std::unordered_map<hasht, vk::PipelineLayout> m_pipelineLayoutMap{};
    };
}