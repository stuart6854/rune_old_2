#include "script_glue.hpp"

#include "common_internal.hpp"
#include "core/engine.hpp"
#include "platform/system_platform.hpp"
#include "scenes/components.hpp"

#include <mono/metadata/loader.h>

#include <entt/entity/entity.hpp>

namespace rune::scriptglue
{
#define RUNE_ADD_INTERNAL_CALL(_funcName) mono_add_internal_call("Rune.InternalCalls::" #_funcName, _funcName)

    void register_functions()
    {
        register_time_functions();
        register_entity_functions();
    }

#pragma region Time

    void register_time_functions()
    {
        RUNE_ADD_INTERNAL_CALL(Time_GetDelta);
        RUNE_ADD_INTERNAL_CALL(Time_GetSinceStartup);
    }

    auto Time_GetDelta() -> float
    {
        return Engine::get().get_delta_time();
    }

    auto Time_GetSinceStartup() -> float
    {
        return f32(Engine::get().get_system<SystemPlatform>()->get_time());
    }

#pragma endregion

#pragma region Entity

    void register_entity_functions()
    {
        RUNE_ADD_INTERNAL_CALL(Entity_Create);
        RUNE_ADD_INTERNAL_CALL(Entity_Destroy);
        RUNE_ADD_INTERNAL_CALL(Entity_GetPosition);
        RUNE_ADD_INTERNAL_CALL(Entity_SetPosition);
    }

    auto Entity_Create() -> Entity
    {
        auto* sceneSystem = Engine::get().get_system<SystemScene>();
        auto entity = sceneSystem->create_entity();
        LOG_INFO("scripting - glue - Created entity {}", Entity(entity));
        return entity;
    }

    void Entity_Destroy(Entity* entity)
    {
        auto* sceneSystem = Engine::get().get_system<SystemScene>();
        sceneSystem->destroy_entity(*entity);
        LOG_INFO("scripting - glue - Destroyed entity {}", Entity(*entity));
    }

    void Entity_GetPosition(Entity* entity, glm::vec3* position)
    {
        auto* sceneSystem = Engine::get().get_system<SystemScene>();
        auto* transform = sceneSystem->get_component<Transform>(*entity);
        *position = transform->position;
    }

    void Entity_SetPosition(Entity* entity, glm::vec3* position)
    {
        auto* sceneSystem = Engine::get().get_system<SystemScene>();
        auto* transform = sceneSystem->get_component<Transform>(*entity);
        transform->position = *position;
    }

#pragma endregion
}