#include "script_glue.hpp"

#include "common_internal.hpp"
#include "core/engine.hpp"
#include "platform/system_platform.hpp"
#include "scenes/components.hpp"

#include <mono/metadata/loader.h>
#include <mono/metadata/reflection.h>

#include <entt/entity/entity.hpp>

#include <functional>
#include <unordered_map>

namespace rune::scriptglue
{
    static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_entityHasComponentFuncs;  // NOLINT

    template <typename... Component>
    void RegisterComponent(MonoImage* image)
    {
        (
            [=]()
            {
                std::string_view typeName = typeid(Component).name();
                sizet pos = typeName.find_last_of(':');
                std::string_view structName = typeName.substr(pos + 1);
                std::string managedTypeName = std::format("Rune.{}", structName);

                MonoType* managedType = mono_reflection_type_from_name(managedTypeName.data(), image);
                if (!managedType)
                {
                    LOG_ERROR("scripting - Could not find component type {}", managedTypeName);
                    return;
                }
                s_entityHasComponentFuncs[managedType] = [](Entity entity)
                { return Engine::get().get_system<SystemScene>()->get_component<Component>(entity) != nullptr; };
            }(),
            ...);
    }

    template <typename... Component>
    static void RegisterComponent(MonoImage* image, ComponentGroup<Component...> /*unused*/)
    {
        RegisterComponent<Component...>(image);
    }

#define RUNE_ADD_INTERNAL_CALL(_funcName) mono_add_internal_call("Rune.InternalCalls::" #_funcName, _funcName)

    void register_functions()
    {
        RUNE_ADD_INTERNAL_CALL(Time_GetDelta);
        RUNE_ADD_INTERNAL_CALL(Time_GetSinceStartup);

        RUNE_ADD_INTERNAL_CALL(Entity_Create);
        RUNE_ADD_INTERNAL_CALL(Entity_Destroy);
        RUNE_ADD_INTERNAL_CALL(Entity_HasComponent);

        RUNE_ADD_INTERNAL_CALL(Transform_GetPosition);
        RUNE_ADD_INTERNAL_CALL(Transform_SetPosition);
    }

    void register_components(MonoImage* image)
    {
        s_entityHasComponentFuncs.clear();
        RegisterComponent(image, AllComponents{});
    }

#pragma region Time

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

    bool Entity_HasComponent(const Entity* entity, MonoReflectionType* componentType)
    {
        auto* managedType = mono_reflection_type_get_type(componentType);
        RUNE_ASSERT(s_entityHasComponentFuncs.contains(managedType));
        return s_entityHasComponentFuncs.at(managedType)(*entity);
    }

#pragma endregion

#pragma region Transform

    void Transform_GetPosition(const Entity* entity, glm::vec3* position)
    {
        auto* sceneSystem = Engine::get().get_system<SystemScene>();
        auto* transform = sceneSystem->get_component<Transform>(*entity);
        *position = transform->position;
    }

    void Transform_SetPosition(const Entity* entity, glm::vec3* position)
    {
        auto* sceneSystem = Engine::get().get_system<SystemScene>();
        auto* transform = sceneSystem->get_component<Transform>(*entity);
        transform->position = *position;
    }

#pragma endregion

}