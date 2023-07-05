#include "scripting/system_scripting.hpp"

#include "common_internal.hpp"

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
    void message_callback(const asSMessageInfo* msg, void* param);

    void SystemScripting::initialise()
    {
        i32 result{};

        m_engine = asCreateScriptEngine();
        RUNE_ASSERT(m_engine != nullptr);

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
        )";

        result = m_module->AddScriptSection("script.as", testScript.c_str());
        CHECK_AS_RESULT(result);
        result = m_module->Build();
        CHECK_AS_RESULT(result);

        const auto* func_decl = "int fib(int)";
        auto* func = m_module->GetFunctionByDecl(func_decl);
        RUNE_ASSERT(func != nullptr);

        // Get a script context instance. Usually you'll want to reuse a previously
        // created instance to avoid the overhead of allocating the instance with
        // each call.
        auto* ctx = m_engine->CreateContext();
        ctx->Prepare(func);  // Prepare the stack

        // Set functions args
        ctx->SetArgDWord(0, 47);  // 47
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
        }

        ctx->Release();
    }

    void SystemScripting::update() {}

    void SystemScripting::shutdown()
    {
        LOG_INFO("Scripting system has shut down.");

        m_engine->ShutDownAndRelease();
    }

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

}