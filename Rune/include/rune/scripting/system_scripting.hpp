#pragma once

#include "common.hpp"
#include "core/system.hpp"

#include <angelscript.h>

namespace rune
{
    class SystemScripting : public ISystem
    {
    public:
        void initialise() override;
        void update() override;
        void shutdown() override;

    private:
    private:
        asIScriptEngine* m_engine{ nullptr };
        asIScriptModule* m_module{ nullptr };
    };
}
