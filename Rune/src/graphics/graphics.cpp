#include "graphics/graphics.hpp"

#include "internal_common.hpp"

namespace rune::graphics
{
    namespace
    {
        gfx::DeviceHandle g_device{};  // NOLINT
    }

    void initialise()
    {
        gfx::set_error_callback([](const char* msg) { LOG_ERROR("{}", msg); });

        gfx::AppInfo appInfo{
            .appName = "rune_sandbox",
            .engineName = "Rune",
        };
        if (!gfx::initialise(appInfo))
        {
            RUNE_THROW_EX("graphics - Failed to initialise graphics layer!");
        }

        gfx::DeviceInfo deviceInfo{
            .deviceFlags = gfx::DeviceFlags_PreferDiscrete,
            .queueFlags = { gfx::QueueFlags_Graphics },
        };

        if (!gfx::create_device(g_device, deviceInfo))
        {
            RUNE_THROW_EX("graphics - Failed to create device!");
        }
    }

    void shutdown()
    {
        gfx::wait_for_device_idle(g_device);
        gfx::destroy_device(g_device);
        gfx::shutdown();
    }

    auto get_device() -> gfx::DeviceHandle
    {
        return g_device;
    }

}