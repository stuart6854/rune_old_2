#include "scripting/system_scripting.hpp"

#include "common_internal.hpp"
#include "io/io.hpp"
#include "script_glue.hpp"
#include "glm/geometric.hpp"

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
        MonoImage* coreAssemblyImage{ nullptr };
    };

    namespace
    {
        auto load_mono_assembly(const std::filesystem::path& assemblyFilename) -> MonoAssembly*
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

    namespace
    {
        void CppFunc()
        {
            LOG_INFO("This is written in C++ (Called from C#)!");
        }

        void NativeLog(MonoString* text, int param)
        {
            char* cStr = mono_string_to_utf8(text);
            LOG_INFO("Text: {}, Param: {}", cStr, param);
            mono_free(cStr);
        }

        /* Just works when C#/C++ types are memory aligned */
        void NativeLog_Vector(glm::vec3* param)
        {
            LOG_INFO("Param: {}, {}, {}", (*param).x, (*param).y, (*param).z);
        }

        void Native_Vector_Cross(glm::vec3* param, glm::vec3* outResult)
        {
            *outResult = glm::cross(*param, glm::vec3(param->x, param->y, -param->z));
        }
    }

    SystemScripting::SystemScripting() : m_pimpl(new Pimpl) {}

    SystemScripting::~SystemScripting() = default;

    void SystemScripting::initialise()
    {
        init_mono();
        load_core_assembly("../../data/Rune-ScriptCore.dll");

        scriptglue::register_functions();

#if 0
        // Retrieve and instiantiate class (with ctor)
        auto* monoClass = mono_class_from_name(m_pimpl->coreAssemblyImage, "Rune", "Test");
        auto* objectInstance = mono_object_new(m_pimpl->appDomain, monoClass);
        mono_runtime_object_init(objectInstance);  // Call (default) ctor, Wse MonoMethod for ctors with params

        // Call method
        auto* printMessageFunc = mono_class_get_method_from_name(monoClass, "PrintMessage", 0);
        mono_runtime_invoke(printMessageFunc, objectInstance, nullptr, nullptr);

        // Call method (w/ params)
        auto* printIntFunc = mono_class_get_method_from_name(monoClass, "PrintInt", 1);
        int value = 5;
        void* params[1] = { &value };
        mono_runtime_invoke(printIntFunc, objectInstance, params, nullptr);

        auto* monoString = mono_string_new(m_pimpl->appDomain, "Hello from C++!");
        auto* printStringFunc = mono_class_get_method_from_name(monoClass, "PrintString", 1);
        void* stringParam = monoString;
        mono_runtime_invoke(printStringFunc, objectInstance, &stringParam, nullptr);
#endif

        // #TODO: Use ManagedThunks for frequently called methods, they are a faster way of calling C# methods (Benchmark?)

        LOG_INFO("Scripting system initialised.");
    }

    void SystemScripting::update() {}

    void SystemScripting::shutdown()
    {
        shutdown_mono();

        LOG_INFO("Scripting system has shut down.");
    }

    void SystemScripting::invoke_method(std::string_view className, std::string_view methodName, void** args)
    {
        auto lastDot = className.find_last_of('.');
        auto namespaceName = std::string(className.substr(0, lastDot));
        auto actualClassName = std::string(className.substr(lastDot + 1, className.size() - lastDot - 1));
        auto* monoClass = mono_class_from_name(m_pimpl->coreAssemblyImage, namespaceName.c_str(), actualClassName.c_str());

        auto* monoMethod = mono_class_get_method_from_name(monoClass, methodName.data(), 1);
        mono_runtime_invoke(monoMethod, nullptr, args, nullptr);
    }

    void SystemScripting::init_mono()
    {
        LOG_INFO("Current Path: {}", std::filesystem::current_path().string());
        mono_set_assemblies_path("lib");

        m_pimpl->rootDomain = mono_jit_init("RuneJitRuntime");
        RUNE_ASSERT(m_pimpl->rootDomain);
    }

    void SystemScripting::shutdown_mono()
    {
        m_pimpl->coreAssembly = nullptr;
        // mono_domain_unload(m_pimpl->appDomain);  // #TODO: Does not work
        m_pimpl->appDomain = nullptr;
        mono_jit_cleanup(m_pimpl->rootDomain);  // This might be unloading all domains??
                                                // https://www.mono-project.com/docs/advanced/embedding/#shutting-down-the-runtime
    }

    void SystemScripting::load_core_assembly(const std::filesystem::path& assemblyFilename)
    {
        m_pimpl->appDomain = mono_domain_create_appdomain(_strdup("RuneScriptDomain"), nullptr);
        RUNE_ASSERT(m_pimpl->appDomain);
        mono_domain_set(m_pimpl->appDomain, true);

        m_pimpl->coreAssembly = load_mono_assembly(assemblyFilename);
        RUNE_ASSERT(m_pimpl->coreAssembly);
        m_pimpl->coreAssemblyImage = mono_assembly_get_image(m_pimpl->coreAssembly);

        print_assembly_types(m_pimpl->coreAssembly);
    }

}