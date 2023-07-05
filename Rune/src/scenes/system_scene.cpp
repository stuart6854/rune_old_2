#include "scenes/system_scene.hpp"

#include "common_internal.hpp"
#include "scenes/components.hpp"
#include "core/engine.hpp"
#include "rendering/system_renderer.hpp"
#include "resources/system_resources.hpp"
#include "utility/primitives.hpp"
#include "utility/yaml-conversions.hpp"

#include <entt/entity/registry.hpp>

#include <yaml-cpp/yaml.h>

#include <glm/gtc/quaternion.hpp>

#include <memory>

namespace rune
{
    struct LoadedSceneData
    {
        std::filesystem::path filename{};
        std::vector<entt::entity> entities{};
    };

    struct SystemScene::Pimpl
    {
        entt::registry registry{};
        std::unordered_map<u64, LoadedSceneData> loadedSceneMap{};

        glm::ivec2 lastCursorPos{};
        glm::vec2 yawPitch{};
    };

    SystemScene::SystemScene() : m_pimpl(new Pimpl) {}

    SystemScene::~SystemScene() = default;

    void SystemScene::initialise()
    {
        clear_scene();

        LOG_INFO("Scene system initialised.");
    }

    void SystemScene::update()
    {
        auto cameraView = m_pimpl->registry.view<Camera, Transform>();
        for (auto entity : cameraView)
        {
            const auto& camera = cameraView.get<Camera>(entity);
            if (!camera.targetWindow)
            {
                continue;
            }

            auto& transform = cameraView.get<Transform>(entity);

            auto* platformSystem = Engine::get().get_system<SystemPlatform>();
            {
                // #TODO: Temp camera input/movement
                auto cursorPos = platformSystem->get_cursor_position(camera.targetWindow);
                glm::vec2 cursorDelta = cursorPos - m_pimpl->lastCursorPos;
                m_pimpl->lastCursorPos = cursorPos;

                if (platformSystem->is_mouse_button_down(camera.targetWindow, Button::Right))
                {
                    glm::vec3 movement{};
                    if (platformSystem->is_key_down(camera.targetWindow, Key::W))
                    {
                        movement += transform.get_forward() * 1.0f;
                    }
                    if (platformSystem->is_key_down(camera.targetWindow, Key::S))
                    {
                        movement += transform.get_forward() * -1.0f;
                    }
                    if (platformSystem->is_key_down(camera.targetWindow, Key::D))
                    {
                        movement += transform.get_right() * 1.0f;
                    }
                    if (platformSystem->is_key_down(camera.targetWindow, Key::A))
                    {
                        movement += transform.get_right() * -1.0f;
                    }
                    if (glm::length(movement) > 0)
                    {
                        transform.position += glm::normalize(movement) * 8.0f * Engine::get().get_delta_time();
                    }

                    m_pimpl->yawPitch += glm::vec2{ cursorDelta.y, cursorDelta.x };
                    m_pimpl->yawPitch.x = glm::clamp(m_pimpl->yawPitch.x, -89.0f, 89.0f);

                    transform.rotation = glm::quat(glm::vec3{ glm::radians(m_pimpl->yawPitch.x), glm::radians(m_pimpl->yawPitch.y), 0.0f });
                }
            }

            glm::mat4 viewMatrix{ 1.0f };
            viewMatrix = glm::translate(viewMatrix, transform.position);
            viewMatrix *= glm::mat4_cast(transform.rotation);
            viewMatrix = glm::inverse(viewMatrix);

            auto windowSize = platformSystem->get_window_size_pixels(camera.targetWindow);
            Engine::get().get_system<SystemRenderer>()->render_camera(
                { camera.targetWindow,
                  windowSize,
                  glm::perspectiveLH_ZO(glm::radians(70.0f), f32(windowSize.x) / f32(windowSize.y), 0.1f, 100.0f),
                  viewMatrix });
        }

        auto view = m_pimpl->registry.view<Transform, StaticRenderer>();
        for (auto entity : view)
        {
            const auto& transform = view.get<Transform>(entity);
            const auto& renderer = view.get<StaticRenderer>(entity);
            if (!renderer.mesh.is_loaded())
            {
                continue;
            }
            std::vector<Material*> materials(renderer.materials.size());
            for (auto i = 0u; i < renderer.materials.size(); ++i)
            {
                materials[i] = renderer.materials[i].get();
            }

            auto worldMatrix = glm::translate(glm::mat4(1.0f), transform.position);
            worldMatrix *= glm::mat4_cast(transform.rotation);
            worldMatrix = glm::scale(worldMatrix, transform.scale);

            Engine::get().get_system<SystemRenderer>()->render_static_mesh(renderer.mesh.get(), materials, worldMatrix);
        }
    }

    void SystemScene::shutdown()
    {
        LOG_INFO("Scene system has shut down.");
    }

    void SystemScene::clear_scene()
    {
        m_pimpl->registry = {};
    }

    void SystemScene::load_scene(std::string_view filename, LoadMethod loadMethod)
    {
        RUNE_UNUSED(loadMethod);

        load_yaml_scene(filename);
    }

    void SystemScene::unload_scene() {}

    void SystemScene::load_yaml_scene(std::filesystem::path filename)
    {
        filename = "../../data" / filename;

        auto scene = YAML::LoadFile(filename.string());
        if (!scene)
        {
            return;
        }

        auto entitiesNode = scene["entities"];
        if (!entitiesNode)
        {
            return;
        }

        auto& sceneData = m_pimpl->loadedSceneMap[STRID(filename.string())];

        auto* resourcesSystem = Engine::get().get_system<SystemResources>();
        for (auto&& entityNode : entitiesNode)
        {
            auto entity = m_pimpl->registry.create();
            sceneData.entities.push_back(entity);

            if (entityNode["Transform"])
            {
                auto node = entityNode["Transform"];
                auto& transform = m_pimpl->registry.emplace<Transform>(entity);
                transform.position = node["position"].as<glm::vec3>();
                transform.rotation = glm::quat(node["rotation"].as<glm::vec3>());
                transform.scale = node["scale"].as<glm::vec3>();
            }

            if (entityNode["StaticRenderer"])
            {
                auto node = entityNode["StaticRenderer"];
                auto& renderer = m_pimpl->registry.emplace<StaticRenderer>(entity);

                auto meshId = node["mesh"].as<u64>();
                renderer.mesh = resourcesSystem->get_ptr<StaticMesh>(meshId);
                resourcesSystem->load(meshId);

                auto materialsNode = node["materials"];
                for (auto&& materialNode : materialsNode)
                {
                    auto materialId = materialNode.as<u64>();
                    auto materialHandle = resourcesSystem->get_ptr<Material>(materialId);
                    renderer.materials.push_back(materialHandle);
                    resourcesSystem->load(materialId);
                }
            }

            if (entityNode["Camera"])
            {
                auto node = entityNode["Camera"];
                auto& camera = m_pimpl->registry.emplace<Camera>(entity);
                camera.targetWindow = Engine::get().get_primary_window();
            }
        }
    }

}