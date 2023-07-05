#include "entry_point.hpp"

#include "common_internal.hpp"
#include "core/platform_detection.hpp"
#include "core/engine.hpp"
#include "core/system_logging.hpp"

#ifdef RUNE_PLATFORM_WINDOWS
    #define NOMINMAX
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

#include <iostream>

#ifdef RUNE_PLATFORM_WINDOWS
int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, PSTR /*lpCmdLine*/, int /*nCmdShow*/)
#else
int main(int /*argc*/, char** /*argv*/)
#endif
{
    using namespace rune;
    EngineConfig config{};
    configure_engine(config);

    Engine engine(config);

    //    try
    {
        engine.run();
    }
#if 0
        catch (RuneException& ex)
    {
                LOG_CRITICAL("critical: runtime exception: {}", ex.what());
                engine.get_system<SystemLogging>()->flush();
                RUNE_DEBUG_BREAK();
                return EXIT_FAILURE;
    }
        catch (std::exception& ex)
        {
            LOG_CRITICAL("critical: runtime exception: {}", ex.what());
            logging::internal::flush();
            RUNE_DEBUG_BREAK();
            return EXIT_FAILURE;
        }
        catch (...)
        {
            LOG_CRITICAL("critical: unknown exception");
            logging::internal::flush();
            RUNE_DEBUG_BREAK();
            return EXIT_FAILURE;
        }
#endif

    return 0;
}