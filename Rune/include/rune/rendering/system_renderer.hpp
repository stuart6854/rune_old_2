#pragma once

#include "common.hpp"
#include "core/system.hpp"
#include "events/system_events.hpp"
#include "platform/system_platform.hpp"
#include "utility/enum_class_flags.hpp"

#include <gfx/gfx.hpp>

#include <glm/ext/matrix_float4x4.hpp>

#include <memory>
#include <unordered_map>

namespace rune
{
    constexpr auto FRAME_BUFFER_COUNT = 2;

#if 0
    enum class RenderFlags : std::uint8_t
    {
        eNone = 0,
        eCastShadows = 1u << 0u,      // Drawn during depth pass
        eReceiveLighting = 1u << 1u,  // Affected by lighting (ambient, diffuse, specular)
        eTranslucent = 1u << 2u,      // Translucent geometry should be drawn last
    };
    RUNE_DEFINE_ENUM_CLASS_BITWISE_OPERATORS(RenderFlags);
#endif

    struct DrawCall
    {
        std::uint32_t material{};
        std::uint32_t mesh{};
        std::uint32_t instance{};
        float distSqr{};  // Distance from camera
    };
#if 0
    using RenderBucket = std::vector<DrawCall>;

    class RenderQueue
    {
    public:
        void add(RenderFlags renderFlags, const glm::mat4& transform = glm::mat4{ 1.0f });

    private:
        std::vector<DrawCall> m_drawCalls{};
    };
#endif

    struct RenderCamera
    {
        WindowHandle targetWindow{};
        glm::u32vec2 targetWindowSize{};
        glm::mat4 projMatrix{ 1.0f };
        glm::mat4 viewMatrix{ 1.0f };
    };

    class StaticMesh;
    class Texture;
    class Material;

    class SystemRenderer : public ISystem
    {
    public:
        void initialise() override;
        void update() override;
        void shutdown() override;

        void render_camera(const RenderCamera& camera);
        void render_static_mesh(const StaticMesh* mesh,
                                const std::vector<Material*>& materials,
                                const glm::mat4& transform = glm::mat4{ 1.0f });

        void flush();

#pragma region Resources

        auto create_static_mesh() -> std::unique_ptr<StaticMesh>;
        void create_skeletal_mesh();
        auto create_texture() -> std::unique_ptr<Texture>;
        auto create_material() -> std::unique_ptr<Material>;

#pragma endregion

        void on_event_recieved(const Event& msg);

        auto get_device() const -> sm::gfx::DeviceHandle { return m_device; }

    private:
        struct RenderMesh
        {
            u32 indexCount{};
            sm::gfx::BufferHandle indexBuffer{};
            std::vector<sm::gfx::BufferHandle> vertexBuffers{};  // Position buffer should always be at index 0
        };

        void flush_camera(const RenderCamera& camera);
        void geometry_pass(sm::gfx::CommandListHandle cmdList);

    private:
        sm::gfx::DeviceHandle m_device{};

        std::unordered_map<WindowHandle, sm::gfx::SwapChainHandle> m_swapchainMap{};

        std::vector<RenderCamera> m_camerasToRender{};
        std::vector<DrawCall> m_drawCalls{};

        std::vector<glm::mat4> m_instances{};
        std::vector<RenderMesh> m_meshes{};
        std::vector<Material*> m_materials{};

        sm::gfx::TextureHandle m_depthAttachment{};

        sm::gfx::BufferHandle m_cameraBuffer{};
        sm::gfx::PipelineHandle m_pipeline{};
        sm::gfx::DescriptorSetHandle m_set{};

        struct FrameData
        {
            std::vector<sm::gfx::FenceHandle> fences{};
            std::vector<sm::gfx::SemaphoreHandle> semaphores{};
            std::vector<sm::gfx::CommandListHandle> commandLists{};
        };
        std::array<FrameData, FRAME_BUFFER_COUNT> m_frames{};
        std::uint32_t m_frameIndex{};
    };

}