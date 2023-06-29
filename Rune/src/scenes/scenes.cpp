#include "scenes/scenes.hpp"

#include "internal_common.hpp"
#include "scenes/components.hpp"
#include "graphics/renderer.hpp"

#include <entt/entity/registry.hpp>

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include <memory>

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

        auto view = registry.view<Transform>();
        for (auto entity : view)
        {
            const auto& transform = view.get<Transform>(entity);

            auto worldMatrix = glm::translate(glm::mat4(1.0f), transform.position);
            worldMatrix = glm::scale(worldMatrix, transform.scale);

            graphics::renderer::render_static_mesh(worldMatrix);
        }
    }

    void new_scene()
    {
        RUNE_ASSERT(g_scenesData != nullptr);

        auto& registry = g_scenesData->registry;
        registry = {};

        auto entity = registry.create();
        auto& transform = registry.emplace<Transform>(entity);
        transform.position = { 0.0f, 0.0f, 0.0f };
        transform.scale = { 0.5f, 0.5f, 0.5f };

        auto entity2 = registry.create();
        auto& transform2 = registry.emplace<Transform>(entity2);
        transform2.position = { 2.0f, 0.0f, 0.0f };
        transform2.scale = { 1.0f, 1.0f, 1.0f };

        auto entity3 = registry.create();
        auto& transform3 = registry.emplace<Transform>(entity3);
        transform3.position = { 0.0f, 2.0f, 0.0f };
        transform3.scale = { 1.0f, 1.0f, 1.0f };

        auto entity4 = registry.create();
        auto& transform4 = registry.emplace<Transform>(entity4);
        transform4.position = { 0.0f, 0.0f, 2.0f };
        transform4.scale = { 1.0f, 1.0f, 1.0f };
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

}