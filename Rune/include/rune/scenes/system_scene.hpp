#pragma once

#include "common.hpp"
#include "core/system.hpp"
#include "components.hpp"

#include <entt/entity/registry.hpp>

#include <vector>
#include <memory>
#include <filesystem>
#include <string_view>

namespace rune
{
    enum class LoadMethod
    {
        Normal,
        Additive,
    };

    using Entity = u32;

    struct LoadedSceneData
    {
        std::filesystem::path filename{};
        std::vector<entt::entity> entities{};
    };

    class SystemScene : public ISystem
    {
    public:
        SystemScene() = default;
        ~SystemScene() override = default;

        void initialise() override;
        void update() override;
        void shutdown() override;

        void clear_scene();

        void load_scene(std::string_view filename, LoadMethod loadMethod = LoadMethod::Normal);
        void unload_scene();

        auto create_entity() -> Entity;
        void destroy_entity(Entity entity);

        template <typename T>
        auto add_component(Entity entity) -> T*;

        template <typename T>
        auto get_component(Entity entity) -> T*;

    private:
        void load_yaml_scene(std::filesystem::path filename);

    private:
        entt::registry m_registry{};
        std::unordered_map<u64, LoadedSceneData> m_loadedSceneMap{};

        glm::ivec2 m_lastCursorPos{};
        glm::vec2 m_yawPitch{};
    };

    template <typename T>
    auto SystemScene::add_component(Entity entity) -> T*
    {
        return &m_registry.get_or_emplace<T>(entt::entity(entity));
    }

    template <typename T>
    auto SystemScene::get_component(Entity entity) -> T*
    {
        return &m_registry.get<T>(entt::entity(entity));
    }
}