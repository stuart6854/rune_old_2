#include "scenes/scenes.hpp"

#include "internal_common.hpp"
#include "scenes/components.hpp"
#include "core/engine.hpp"
#include "platform/platform.hpp"
#include "graphics/renderer/renderer.hpp"
#include "resources/manager.hpp"
#include "utility/primitives.hpp"

#include <entt/entity/registry.hpp>

#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/quaternion.hpp>

#include <memory>

#define RUNE_CREATE_TEST_ENTITY(_position, _scale, _mesh, _mat)    \
    {                                                              \
        auto entity = registry.create();                           \
        auto& transform = registry.emplace<Transform>(entity);     \
        transform.position = _position;                            \
        transform.scale = _scale;                                  \
        auto& renderer = registry.emplace<StaticRenderer>(entity); \
        renderer.mesh = _mesh;                                     \
        renderer.materials = { _mat };                             \
    }

namespace rune::scenes
{
    struct ScenesData
    {
        entt::registry registry{};
        std::unordered_map<std::string, int> loadedSceneMap{};
    };
    std::unique_ptr<ScenesData> g_scenesData{ nullptr };  // NOLINT

    std::pair<f64, f64> g_lastCursorPos{};
    glm::vec2 g_yawPitch{};

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

        auto cameraView = registry.view<Camera, Transform>();
        for (auto entity : cameraView)
        {
            const auto& camera = cameraView.get<Camera>(entity);
            if (!camera.targetWindow)
            {
                continue;
            }

            auto& transform = cameraView.get<Transform>(entity);

            {
                // #TODO: Temp camera input/movement
                auto cursorPos = platform::get_cursor_position(camera.targetWindow);
                glm::vec2 cursorDelta = { cursorPos.first - g_lastCursorPos.first, cursorPos.second - g_lastCursorPos.second };
                g_lastCursorPos = cursorPos;

                if (platform::is_mouse_button_down(camera.targetWindow, platform::Button::Right))
                {
                    glm::vec3 movement{};
                    if (platform::is_key_down(camera.targetWindow, platform::Key::W))
                    {
                        movement += transform.get_forward() * 1.0f;
                    }
                    if (platform::is_key_down(camera.targetWindow, platform::Key::S))
                    {
                        movement += transform.get_forward() * -1.0f;
                    }
                    if (platform::is_key_down(camera.targetWindow, platform::Key::D))
                    {
                        movement += transform.get_right() * 1.0f;
                    }
                    if (platform::is_key_down(camera.targetWindow, platform::Key::A))
                    {
                        movement += transform.get_right() * -1.0f;
                    }
                    if (glm::length(movement) > 0)
                    {
                        transform.position += glm::normalize(movement) * 8.0f * engine::get_delta_time();
                    }

                    g_yawPitch += glm::vec2{ cursorDelta.y, cursorDelta.x };
                    g_yawPitch.x = glm::clamp(g_yawPitch.x, -89.0f, 89.0f);

                    transform.rotation = glm::quat(glm::vec3{ glm::radians(g_yawPitch.x), glm::radians(g_yawPitch.y), 0.0f });
                }
            }

            glm::mat4 viewMatrix{ 1.0f };
            viewMatrix = glm::translate(viewMatrix, transform.position);
            viewMatrix *= glm::mat4_cast(transform.rotation);
            viewMatrix = glm::inverse(viewMatrix);

            auto windowSize = platform::get_window_size_pixels(camera.targetWindow);
            graphics::renderer::render_camera(
                { camera.targetWindow,
                  windowSize,
                  glm::perspectiveLH_ZO(glm::radians(70.0f), f32(windowSize.x) / f32(windowSize.y), 0.1f, 100.0f),
                  viewMatrix });
        }

        auto view = registry.view<Transform, StaticRenderer>();
        for (auto entity : view)
        {
            const auto& transform = view.get<Transform>(entity);
            const auto& renderer = view.get<StaticRenderer>(entity);
            if (!renderer.mesh.is_loaded())
            {
                continue;
            }
            std::vector<graphics::renderer::Material*> materials(renderer.materials.size());
            for (auto i = 0; i < renderer.materials.size(); ++i)
            {
                materials[i] = renderer.materials[i].get();
            }

            auto worldMatrix = glm::translate(glm::mat4(1.0f), transform.position);
            worldMatrix *= glm::mat4_cast(transform.rotation);
            worldMatrix = glm::scale(worldMatrix, transform.scale);

            graphics::renderer::render_static_mesh(renderer.mesh.get(), materials, worldMatrix);
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
        auto material = resources::get_ptr<graphics::renderer::Material>(23459839);
        resources::load(23459839);

        RUNE_CREATE_TEST_ENTITY(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), planeMesh, material);
        RUNE_CREATE_TEST_ENTITY(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.01f, 0.01f, 0.01f), graveMesh, material);
        RUNE_CREATE_TEST_ENTITY(glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(0.01f, 0.01f, 0.01f), roadSignMesh, material);
        RUNE_CREATE_TEST_ENTITY(glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), cubeMesh, material);
        RUNE_CREATE_TEST_ENTITY(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.01f, 0.01f, 0.01f), rockMesh, material);

        {
            auto cameraEntity = registry.create();
            auto& camera = registry.emplace<Camera>(cameraEntity);
            camera.targetWindow = engine::get_primary_window();
            auto& transform = registry.emplace<Transform>(cameraEntity);
            transform.position = {};
            transform.rotation = {};
        }
    }

}