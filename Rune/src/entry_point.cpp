#include "entry_point.hpp"

#include "core/platform_detection.hpp"

#ifdef RUNE_PLATFORM_WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
#endif

#ifdef RUNE_PLATFORM_WINDOWS
int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, PSTR /*lpCmdLine*/, int /*nCmdShow*/)
#else
int main(int /*argc*/, char** /*argv*/)
#endif
{
    EngineConfig config{};
    configure_engine(config);

    return 0;
}