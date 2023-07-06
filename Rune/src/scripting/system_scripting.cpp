#include "scripting/system_scripting.hpp"

#include "common_internal.hpp"
#include "io/io.hpp"
#include "script_glue.hpp"
#include "glm/geometric.hpp"

#include <entt/entity/entity.hpp>

#include <mono/jit/jit.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/assembly.h>

#include <cstring>

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

        MonoAssembly* appAssembly{ nullptr };
        MonoImage* appAssemblyImage{ nullptr };

        std::filesystem::path coreAssemblyFilename;
        std::filesystem::path appAssemblyFilename;
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
        scriptglue::register_functions();

        if (!load_core_assembly("../../data/Rune-ScriptCore.dll"))
        {
            RUNE_THROW_EX("Failed to load Rune-ScriptCore assembly.");
            return;
        }

        // #TODO: Get engine data dir + app script assembly filename from somewhere (Config)
        auto scriptModulePath = std::filesystem::current_path() / "../../data/Sandbox-ScriptAssembly.dll";
        if (!load_app_assembly(scriptModulePath))
        {
            RUNE_THROW_EX("Failed to load app assembly.");
            return;
        }

        scriptglue::register_components(m_pimpl->coreAssemblyImage);

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
        auto* monoClass = mono_class_from_name(m_pimpl->appAssemblyImage, namespaceName.c_str(), actualClassName.c_str());

        auto* monoMethod = mono_class_get_method_from_name(monoClass, methodName.data(), 1);
        mono_runtime_invoke(monoMethod, nullptr, args, nullptr);
    }

    void SystemScripting::init_mono()
    {
        mono_set_assemblies_path("lib");

        m_pimpl->rootDomain = mono_jit_init("RuneJitRuntime");
        RUNE_ASSERT(m_pimpl->rootDomain);

        mono_thread_set_main(mono_thread_current());
    }

    void SystemScripting::shutdown_mono()
    {
        mono_domain_set(mono_get_root_domain(), false);

        mono_domain_unload(m_pimpl->appDomain);
        m_pimpl->appDomain = nullptr;

        mono_jit_cleanup(m_pimpl->rootDomain);
        m_pimpl->rootDomain = nullptr;
    }

    bool SystemScripting::load_core_assembly(const std::filesystem::path& assemblyFilename)
    {
        m_pimpl->appDomain = mono_domain_create_appdomain(_strdup("RuneScriptDomain"), nullptr);
        mono_domain_set(m_pimpl->appDomain, true);

        m_pimpl->coreAssemblyFilename = assemblyFilename;
        m_pimpl->coreAssembly = load_mono_assembly(assemblyFilename);
        if (!m_pimpl->coreAssembly)
        {
            return false;
        }

        m_pimpl->coreAssemblyImage = mono_assembly_get_image(m_pimpl->coreAssembly);

        return true;
    }

    bool SystemScripting::load_app_assembly(const std::filesystem::path& assemblyFilename)
    {
        m_pimpl->appAssemblyFilename = assemblyFilename;
        m_pimpl->appAssembly = load_mono_assembly(assemblyFilename);
        if (!m_pimpl->appAssembly)
        {
            return false;
        }

        m_pimpl->appAssemblyImage = mono_assembly_get_image(m_pimpl->appAssembly);

        // #TODO: Setup filewatcher

        return true;
    }

}