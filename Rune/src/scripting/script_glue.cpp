#include "script_glue.hpp"

#include "common_internal.hpp"
#include "core/engine.hpp"

#include <entt/entity/entity.hpp>

#define CHECK_RESULT(_x)                 \
    auto COMBINE(result, __LINE__) = _x; \
    RUNE_ASSERT(COMBINE(result, __LINE__) >= 0)

namespace rune::scriptglue
{
#pragma region Entity

    void register_entity_types()
    {
#if 0
        CHECK_RESULT(engine->SetDefaultNamespace(""));
        CHECK_RESULT(engine->RegisterObjectType("Entity", sizeof(entt::entity), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<entt::entity>()));
        CHECK_RESULT(engine->RegisterObjectMethod("Entity", "void destroy()", asFUNCTION(entity_destroy), asCALL_CDECL_OBJLAST));
        CHECK_RESULT(engine->RegisterObjectMethod("Entity", "vec3 get_position()", asFUNCTION(entity_position_get), asCALL_CDECL_OBJLAST));
        CHECK_RESULT(
            engine->RegisterObjectMethod("Entity", "void set_position(vec3)", asFUNCTION(entity_position_set), asCALL_CDECL_OBJLAST));

        CHECK_RESULT(engine->SetDefaultNamespace("Entity"));
        CHECK_RESULT(engine->RegisterGlobalFunction("Entity create()", asFUNCTION(entity_create), asCALL_CDECL));
#endif
    }

    auto entity_create() -> Entity
    {
        auto* sceneSystem = Engine::get().get_system<SystemScene>();
        auto entity = sceneSystem->create_entity();
        LOG_INFO("scripting - glue - Created entity {}", Entity(entity));
        return entity;
    }

    void entity_destroy(Entity* entity)
    {
        auto* sceneSystem = Engine::get().get_system<SystemScene>();
        sceneSystem->destroy_entity(*entity);
        LOG_INFO("scripting - glue - Destroyed entity {}", Entity(*entity));
    }

    auto entity_position_get(Entity* entity) -> glm::vec3
    {
        auto* sceneSystem = Engine::get().get_system<SystemScene>();
        sceneSystem->destroy_entity(*entity);
        return {};
    }

    void entity_position_set(Entity* entity, glm::vec3 position)
    {
        RUNE_UNUSED(position);
        auto* sceneSystem = Engine::get().get_system<SystemScene>();
        sceneSystem->destroy_entity(*entity);
    }

#pragma endregion
}