#pragma once

#include "common.hpp"
#include "core/system.hpp"

#include <memory>
#include <filesystem>

namespace rune
{
    class SystemScripting : public ISystem
    {
    public:
        SystemScripting();
        ~SystemScripting() override;

        void initialise() override;
        void update() override;
        void shutdown() override;

        void invoke_method(std::string_view className, std::string_view methodName, void** args);

    private:
        void init_mono();
        void shutdown_mono();

        bool load_core_assembly(const std::filesystem::path& assemblyFilename);
        bool load_app_assembly(const std::filesystem::path& assemblyFilename);

    private:
        struct Pimpl;
        std::unique_ptr<Pimpl> m_pimpl;
#if 0
        asIScriptEngine* m_engine{ nullptr };
        asIScriptModule* m_module{ nullptr };
#endif
    };
}
