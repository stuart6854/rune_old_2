#include "entry_point.hpp"

#include "core/platform_detection.hpp"
#include "core/internal/internal_engine.hpp"
#include "core/internal/internal_logging.hpp"

#ifdef RUNE_PLATFORM_WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
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

    try
    {
        engine::internal::initialise();
        // #TODO: Run engine
        engine::internal::shutdown();
    }
    catch (...)
    {
        std::cerr << "Error: unknown exception" << std::endl;
        // #TODO: Ensure logging code is flushed (log files written, etc.)
        logging::internal::flush();
        return EXIT_FAILURE;
    }

    return 0;
}