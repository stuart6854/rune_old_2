#pragma once

#include "common.hpp"
#include "static_mesh.hpp"
#include "texture.hpp"
#include "material.hpp"
#include "platform/platform.hpp"
#include "utility/enum_class_flags.hpp"

#include "glm/ext/matrix_float4x4.hpp"

#include <memory>

namespace rune::graphics::renderer
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
        platform::WindowHandle targetWindow{};
        glm::u32vec2 targetWindowSize{};
        glm::mat4 projMatrix{ 1.0f };
        glm::mat4 viewMatrix{ 1.0f };
    };

    void initialise();
    void shutdown();

    void render_camera(const RenderCamera& camera);
    void render_static_mesh(const StaticMesh* mesh, const glm::mat4& transform = glm::mat4{ 1.0f });

    void flush_renders();

#pragma region Resources

    auto create_static_mesh() -> std::unique_ptr<StaticMesh>;
    void create_skeletal_mesh();
    auto create_texture() -> std::unique_ptr<Texture>;
    auto create_material() -> std::unique_ptr<Material>;

#pragma endregion

}