#include "scenes/scenes.hpp"

#include "internal_common.hpp"
#include "scenes/components.hpp"

#include <entt/entity/registry.hpp>

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
            LOG_INFO("Entity with Transform: {}", u64(entity));
        }
    }

    void new_scene()
    {
        RUNE_ASSERT(g_scenesData != nullptr);

        auto& registry = g_scenesData->registry;
        registry = {};

        auto entity = registry.create();
        registry.emplace<Transform>(entity);

        auto entity2 = registry.create();
        registry.emplace<Transform>(entity2);
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