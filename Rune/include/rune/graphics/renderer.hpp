#pragma once

#include "common.hpp"
#include "platform/platform.hpp"

#include <gfx/gfx.hpp>

#include <glm/ext/matrix_float4x4.hpp>

namespace rune::graphics
{
    using namespace sm;

    constexpr auto FRAME_BUFFER_COUNT = 2;

    struct DrawCall
    {
        std::uint32_t material{};
        std::uint32_t mesh{};
        std::uint32_t instance{};
        float distSqr{};  // Distance from camera
    };

    class RenderQueue
    {
    public:
        void add(const glm::mat4& transform = glm::mat4{ 1.0f });

    private:
        std::vector<DrawCall> m_drawCalls{};
    };

    struct RenderCamera
    {
        platform::WindowHandle targetWindow{};
        glm::u32vec2 targetWindowSize{};
        glm::mat4 projMatrix{ 1.0f };
        glm::mat4 viewMatrix{ 1.0f };
    };

    class Renderer
    {
    public:
        Renderer() = default;
        ~Renderer() = default;

        void initialise();
        void shutdown();

        void render_camera(const RenderCamera& camera);
        void render_static_mesh(/*StaticMesh* mesh, */ const glm::mat4& transform = glm::mat4{ 1.0f });

        void flush_renders();

#pragma region Resources

        void create_static_mesh();
        void create_skeletal_mesh();
        void create_texture();
        void create_material();

#pragma endregion

    private:
        void flush_camera(const RenderCamera& camera);

        void geometry_pass(gfx::CommandListHandle cmdList);

    private:
        std::unordered_map<platform::WindowHandle, gfx::SwapChainHandle> m_swapchainMap{};

        std::vector<RenderCamera> m_camerasToRender{};
        std::vector<DrawCall> m_drawCalls{};

        std::vector<glm::mat4> m_instances{};

        gfx::BufferHandle m_cameraBuffer{};
        gfx::PipelineHandle m_pipeline{};
        gfx::DescriptorSetHandle m_set{};

        struct FrameData
        {
            std::vector<gfx::FenceHandle> fences{};
            std::vector<gfx::SemaphoreHandle> semaphores{};
            std::vector<gfx::CommandListHandle> commandLists{};
        };
        std::array<FrameData, FRAME_BUFFER_COUNT> m_frames{};
        std::uint32_t m_frameIndex{};
    };
}