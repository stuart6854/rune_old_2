#pragma once

#if defined(_WIN32)
    #define RUNE_PLATFORM_WINDOWS
#elif defined(__linux__)
    #define RUNE_PLATFORM_LINUX
#else
    #error Unsupported platform!
#endif