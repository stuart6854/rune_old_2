#include "entry_point.hpp"

#include "core/platform_detection.hpp"
#include "core/internal/internal_engine.hpp"
#include "core/internal/internal_logging.hpp"

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

    //    try
    {
        logging::internal::initialise(config.logDirPath);
        engine::internal::initialise(config);
        engine::internal::run();
        engine::internal::shutdown();
        logging::internal::shutdown();
    }
    //    catch (std::exception& ex)
    //    {
    //        LOG_CRITICAL("critical: runtime exception: {}", ex.what());
    //        logging::internal::flush();
    //        RUNE_DEBUG_BREAK();
    //        return EXIT_FAILURE;
    //    }
    //    catch (...)
    //    {
    //        LOG_CRITICAL("critical: unknown exception");
    //        logging::internal::flush();
    //        RUNE_DEBUG_BREAK();
    //        return EXIT_FAILURE;
    //    }

    return 0;
}