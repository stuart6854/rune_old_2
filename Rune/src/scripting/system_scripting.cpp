#include "scripting/system_scripting.hpp"

#include "common_internal.hpp"
#include "io/io.hpp"
#include "script_glue.hpp"

#include <entt/entity/entity.hpp>

#include <mono/metadata/assembly.h>
#include <mono/jit/jit.h>

#include <string.h>

#define CHECK_AS_RESULT(_result) RUNE_ASSERT(_result >= 0)

// #TODO: "Rune Scripting Library"
// - Core (Quit app, etc.)
// - Time (delta, current, etc.)
// - Maths (Vec2/3/4, Rect, etc.)
// - Scene (Load/Unload scene, etc.)
// - Entity (Entity, Components, ect.)
// - Input (Keys, Buttons, Cursor, Gamepad, etc.)
// - Debug (Log, Assert, Draw, etc.)

namespace rune
{
    struct SystemScripting::Pimpl
    {
        MonoDomain* rootDomain{ nullptr };
        MonoDomain* appDomain{ nullptr };
        MonoAssembly* coreAssembly{ nullptr };
    };

    //    void message_callback(const asSMessageInfo* msg, void* param);
    auto load_assembly(const std::filesystem::path& assemblyFilename) -> MonoAssembly*;
    void print_assembly_types(MonoAssembly* assembly);

    SystemScripting::SystemScripting() : m_pimpl(new Pimpl) {}

    SystemScripting::~SystemScripting() = default;

    void SystemScripting::initialise()
    {
#if 0
        i32 result{};

        m_engine = asCreateScriptEngine();
        RUNE_ASSERT(m_engine != nullptr);

        scriptglue::register_entity_types(m_engine);

        result = m_engine->SetMessageCallback(asFUNCTION(message_callback), nullptr, asCALL_CDECL);
        CHECK_AS_RESULT(result);

        m_module = m_engine->GetModule("module", asGM_ALWAYS_CREATE);
        RUNE_ASSERT(m_module != nullptr);

        LOG_INFO("Scripting system initialised.");

        std::string testScript = R"(
int fib(int n)
{
    if(n <= 1) return n;
    return fib(n - 1) + fib(n - 2);
}

Entity test()
{
    Entity e1 = Entity::create();
    Entity e2 = Entity::create();
    e1.destroy();
    return e2;
}

void update(Entity entity)
{
    auto position = entity.get_position();
    position += vec3(0.0f, 0.0f, 0.0f) * Time::get_delta();
    entity.set_position(position);
}
        )";

        result = m_module->AddScriptSection("script.as", testScript.c_str());
        CHECK_AS_RESULT(result);
        result = m_module->Build();
        CHECK_AS_RESULT(result);

        const auto* func_decl = "int fib(int)";
        //        const auto* func_decl = "Entity test()";
        auto* func = m_module->GetFunctionByDecl(func_decl);
        RUNE_ASSERT(func != nullptr);

        // Get a script context instance. Usually you'll want to reuse a previously
        // created instance to avoid the overhead of allocating the instance with
        // each call.
        auto* ctx = m_engine->CreateContext();
        ctx->Prepare(func);  // Prepare the stack

        // Set functions args
        ctx->SetArgDWord(0, 30);  // 47
        // ctx->SetArgQWord(arg, value);
        // ctx->SetArgFloat(arg, value);
        // ctx->SetArgDouble(arg, value);
        // ctx->SetArgByte(arg, value);
        // ctx->SetArgWord(arg, value);
        // ctx->SetArgAddress(arg, value); // Primitive Ref
        // ctx->SetArgObject(arg, value); // Non-Primitive Ref

        result = ctx->Execute();
        CHECK_AS_RESULT(result);
        if (result == asEXECUTION_FINISHED)
        {
            // The return value is only valid if the execution finished successfully
            asDWORD ret = ctx->GetReturnDWord();
            LOG_INFO("scripting - Return value: {}", ret);
            // auto* ret = ctx->GetReturnObject();
            // entt::entity entity = *(entt::entity*)ret;
            // LOG_INFO("scripting - Returned entity: {}", Entity(entity));
        }

        ctx->Release();
#endif

        init_mono();
    }

    void SystemScripting::update()
    {
#if 0
        i32 result{};

        const auto* func_decl = "void update(Entity)";
        auto* func = m_module->GetFunctionByDecl(func_decl);
        RUNE_ASSERT(func != nullptr);

        auto* ctx = m_engine->CreateContext();
        ctx->Prepare(func);  // Prepare the stack

        Entity entity{ 0 };
        ctx->SetArgAddress(0, &entity);

        result = ctx->Execute();
        CHECK_AS_RESULT(result);

        ctx->Release();
#endif
    }

    void SystemScripting::shutdown()
    {
#if 0
        m_engine->ShutDownAndRelease();
#endif

        shutdown_mono();

        LOG_INFO("Scripting system has shut down.");
    }

    void SystemScripting::init_mono()
    {
        LOG_INFO("Current Path: {}", std::filesystem::current_path().string());
        mono_set_assemblies_path("lib");

        m_pimpl->rootDomain = mono_jit_init("RuneJitRuntime");
        RUNE_ASSERT(m_pimpl->rootDomain);

        m_pimpl->appDomain = mono_domain_create_appdomain(_strdup("RuneScriptDomain"), nullptr);
        RUNE_ASSERT(m_pimpl->appDomain);
        mono_domain_set(m_pimpl->appDomain, true);

        m_pimpl->coreAssembly = load_assembly("../../data/Rune-ScriptCore.dll");
        RUNE_ASSERT(m_pimpl->coreAssembly);

        print_assembly_types(m_pimpl->coreAssembly);

        MonoImage* assemblyImage = mono_assembly_get_image(m_pimpl->coreAssembly);
        auto* monoClass = mono_class_from_name(assemblyImage, "Rune", "Test");

        // Creating managed object
        auto* objectInstance = mono_object_new(m_pimpl->appDomain, monoClass);
        mono_runtime_object_init(objectInstance);  // Call (default) ctor, Wse MonoMethod for ctors with params

        // Call function
        auto* printMessageFunc = mono_class_get_method_from_name(monoClass, "PrintMessage", 0);
        mono_runtime_invoke(printMessageFunc, objectInstance, nullptr, nullptr);

        // Call function (w/ params)
        auto* printIntFunc = mono_class_get_method_from_name(monoClass, "PrintInt", 1);
        int value = 5;
        void* params[1] = { &value };
        mono_runtime_invoke(printIntFunc, objectInstance, params, nullptr);

        auto* monoString = mono_string_new(m_pimpl->appDomain, "Hello from C++!");
        auto* printStringFunc = mono_class_get_method_from_name(monoClass, "PrintString", 1);
        void* stringParam = monoString;
        mono_runtime_invoke(printStringFunc, objectInstance, &stringParam, nullptr);

        // #TODO: Use ManagedThunks for frequently called methods, they are a faster way of calling C# methods (Benchmark?)
    }

    void SystemScripting::shutdown_mono()
    {
        m_pimpl->coreAssembly = nullptr;
        // mono_domain_unload(m_pimpl->appDomain);  // #TODO: Does not work
        m_pimpl->appDomain = nullptr;
        mono_jit_cleanup(m_pimpl->rootDomain);  // This might be unloading all domains??
                                                // https://www.mono-project.com/docs/advanced/embedding/#shutting-down-the-runtime
    }

#if 0
    void message_callback(const asSMessageInfo* msg, void* param)
    {
        RUNE_UNUSED(param);

        if (msg->type == asMSGTYPE_ERROR)
        {
            LOG_ERROR("Scripting - {} ({}, {}) : {}", msg->section, msg->row, msg->col, msg->message);
        }
        else if (msg->type == asMSGTYPE_WARNING)
        {
            LOG_WARN("Scripting - {} ({}, {}) : {}", msg->section, msg->row, msg->col, msg->message);
        }
        else
        {
            LOG_INFO("Scripting - {} ({}, {}) : {}", msg->section, msg->row, msg->col, msg->message);
        }
    }
#endif

    auto load_assembly(const std::filesystem::path& assemblyFilename) -> MonoAssembly*
    {
        auto assemblyData = io::read_binary<char>(assemblyFilename);

        MonoImageOpenStatus status{};
        auto* image = mono_image_open_from_data_full(assemblyData->data(), u32(assemblyData->size()), true, &status, false);
        if (status != MONO_IMAGE_OK)
        {
            //            const auto* errorMsg = mono_image_strerror(status);
            return nullptr;
        }

        auto* assembly = mono_assembly_load_from_full(image, assemblyFilename.string().c_str(), &status, false);
        mono_image_close(image);

        return assembly;
    }

    void print_assembly_types(MonoAssembly* assembly)
    {
        MonoImage* image = mono_assembly_get_image(assembly);
        // #TODO: Try print attributes: MONO_TABLE_CUSTOMATTRIBUTE
        const auto* typeDefinitionTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
        i32 numTypes = mono_table_info_get_rows(typeDefinitionTable);
        for (auto i = 0; i < numTypes; ++i)
        {
            u32 cols[MONO_TYPEDEF_SIZE];
            mono_metadata_decode_row(typeDefinitionTable, i, cols, MONO_TYPEDEF_SIZE);

            const auto* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
            const auto* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

            LOG_DEBUG("{}.{}", nameSpace, name);
        }
    }

}