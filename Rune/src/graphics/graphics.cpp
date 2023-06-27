#include "graphics/graphics.hpp"

#include "internal_common.hpp"

namespace rune::graphics
{
    namespace
    {
        gfx::DeviceHandle g_device{};  // NOLINT
    }

    void initialise(void* platformWindowHandle)
    {
        gfx::AppInfo appInfo{
            .appName = "rune_sandbox",
            .engineName = "Rune",
        };
        if (!gfx::initialise(appInfo))
        {
            throw std::runtime_error("graphics - Failed to initialise graphics layer!");
        }

        gfx::DeviceInfo deviceInfo{
            .deviceFlags = gfx::DeviceFlags_PreferDiscrete,
            .queueFlags = { gfx::QueueFlags_Graphics },
        };

        if (!gfx::create_device(g_device, deviceInfo))
        {
            throw std::runtime_error("graphics - Failed to create device!");
        }

        gfx::SwapChainInfo swapChainInfo{
            .platformWindowHandle = platformWindowHandle,
            .initialWidth = 800,
            .initialHeight = 600,
        };
        gfx::SwapChainHandle swapChainHandle{};
        if (!gfx::create_swap_chain(swapChainHandle, g_device, swapChainInfo))
        {
            throw std::runtime_error("graphics - Failed to create swap chain!");
        }
    }

    void shutdown()
    {
        gfx::destroy_device(g_device);
        gfx::shutdown();
    }

}