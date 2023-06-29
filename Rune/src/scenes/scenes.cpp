#include "scenes/scenes.hpp"

#include "internal_common.hpp"
#include "scenes/components.hpp"
#include "graphics/renderer/renderer.hpp"
#include "utility/primitives.hpp"

#include <entt/entity/registry.hpp>

#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include <memory>

#define RUNE_CREATE_TEST_ENTITY(_position, _scale, _mesh)          \
    {                                                              \
        auto entity = registry.create();                           \
        auto& transform = registry.emplace<Transform>(entity);     \
        transform.position = _position;                            \
        transform.scale = _scale;                                  \
        auto& renderer = registry.emplace<StaticRenderer>(entity); \
        renderer.mesh = _mesh;                                     \
    }

namespace rune::scenes
{
    struct ScenesData
    {
        entt::registry registry{};
        std::unordered_map<std::string, int> loadedSceneMap{};
    };
    std::unique_ptr<ScenesData> g_scenesData{ nullptr };  // NOLINT

    void initialise()
    {
        RUNE_ASSERT(g_scenesData == nullptr);

        g_scenesData = std::make_unique<ScenesData>();

        new_scene();
    }

    void shutdown()
    {
        RUNE_ASSERT(g_scenesData != nullptr);

        g_scenesData = nullptr;
    }

    void update()
    {
        RUNE_ASSERT(g_scenesData != nullptr);

        auto& registry = g_scenesData->registry;

        auto view = registry.view<Transform, StaticRenderer>();
        for (auto entity : view)
        {
            const auto& transform = view.get<Transform>(entity);
            const auto& renderer = view.get<StaticRenderer>(entity);

            auto worldMatrix = glm::translate(glm::mat4(1.0f), transform.position);
            worldMatrix = glm::scale(worldMatrix, transform.scale);

            graphics::renderer::render_static_mesh(renderer.mesh, worldMatrix);
        }
    }

    void new_scene()
    {
        RUNE_ASSERT(g_scenesData != nullptr);

        auto& registry = g_scenesData->registry;
        registry = {};
    }

    void load_scene(std::string_view filename, LoadMethod loadMethod)
    {
        RUNE_UNUSED(filename);
        RUNE_UNUSED(loadMethod);

        RUNE_ASSERT(g_scenesData != nullptr);
    }

    void unload_scene()
    {
        RUNE_ASSERT(g_scenesData != nullptr);
    }

    void new_test_scene()
    {
        new_scene();
        auto& registry = g_scenesData->registry;

        auto mesh = utility::primitives::generate_plane({ 5, 5 }, { -2.5f, -2.5f }, { 1, 1 });

        RUNE_CREATE_TEST_ENTITY(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), mesh);
        RUNE_CREATE_TEST_ENTITY(glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), mesh);
        RUNE_CREATE_TEST_ENTITY(glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), mesh);
        RUNE_CREATE_TEST_ENTITY(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(1.0f, 1.0f, 1.0f), mesh);
    }

}