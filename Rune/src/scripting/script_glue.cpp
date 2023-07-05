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

    void register_entity_types(asIScriptEngine* engine)
    {
        CHECK_RESULT(engine->SetDefaultNamespace(""));
        CHECK_RESULT(engine->RegisterObjectType("Entity", sizeof(entt::entity), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<entt::entity>()));
        CHECK_RESULT(engine->RegisterObjectMethod("Entity", "void destroy()", asFUNCTION(destroy_entity), asCALL_CDECL_OBJLAST));

        CHECK_RESULT(engine->SetDefaultNamespace("Entity"));
        CHECK_RESULT(engine->RegisterGlobalFunction("Entity create()", asFUNCTION(create_entity), asCALL_CDECL));
    }

    auto create_entity() -> Entity
    {
        auto* sceneSystem = Engine::get().get_system<SystemScene>();
        auto entity = sceneSystem->create_entity();
        LOG_INFO("scripting - glue - Created entity {}", Entity(entity));
        return entity;
    }

    void destroy_entity(Entity* entity)
    {
        auto* sceneSystem = Engine::get().get_system<SystemScene>();
        sceneSystem->destroy_entity(*entity);
        LOG_INFO("scripting - glue - Destroyed entity {}", Entity(*entity));
    }

#pragma endregion

}