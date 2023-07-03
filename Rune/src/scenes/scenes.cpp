#include "scenes/scenes.hpp"

#include "internal_common.hpp"
#include "scenes/components.hpp"
#include "graphics/renderer/renderer.hpp"
#include "resources/manager.hpp"
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
            if (!renderer.mesh.is_loaded())
            {
                continue;
            }

            auto worldMatrix = glm::translate(glm::mat4(1.0f), transform.position);
            worldMatrix = glm::scale(worldMatrix, transform.scale);

            graphics::renderer::render_static_mesh(renderer.mesh.get(), worldMatrix);
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

        auto planeMesh = resources::get_ptr<graphics::renderer::StaticMesh>(STRID("builtin_plane_5m"));
        auto cubeMesh = resources::get_ptr<graphics::renderer::StaticMesh>(STRID("builtin_unit_cube"));
        auto graveMesh = resources::get_ptr<graphics::renderer::StaticMesh>(6876786);
        auto roadSignMesh = resources::get_ptr<graphics::renderer::StaticMesh>(23455);
        auto rockMesh = resources::get_ptr<graphics::renderer::StaticMesh>(1998);
        resources::load(6876786);
        resources::load(23455);
        resources::load(1998);

        RUNE_CREATE_TEST_ENTITY(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), planeMesh);
        RUNE_CREATE_TEST_ENTITY(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.01f, 0.01f, 0.01f), graveMesh);
        RUNE_CREATE_TEST_ENTITY(glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(0.01f, 0.01f, 0.01f), roadSignMesh);
        RUNE_CREATE_TEST_ENTITY(glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), cubeMesh);
        RUNE_CREATE_TEST_ENTITY(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.01f, 0.01f, 0.01f), rockMesh);
    }

}