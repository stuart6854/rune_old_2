#include <rune/rune.hpp>

using namespace rune;

class ApplicationSandbox : public IApplication
{
public:
    void initialise(Engine& /*engine*/) override
    {
        m_primaryWindow = platform::create_window("Sandbox", 800, 600);

        m_renderDevice = rhi::Device::create();

        rhi::SurfaceDecl surfaceDecl{
            .DisplayHandle = m_primaryWindow->native_display_handle(),
            .SurfaceHandle = m_primaryWindow->native_surface_handle(),
            .Size = m_primaryWindow->fb_size(),
        };
        m_renderSurface = rhi::Surface::create(m_renderDevice, surfaceDecl);

        m_cmdList = rhi::CommandList::create(m_renderDevice);
        m_fence = rhi::Fence::create(m_renderDevice);

        RUNE_CLIENT_INFO("Sandbox initialised.");
    }

    void shutdown(Engine& /*engine*/) override
    {
        m_cmdList = nullptr;
        m_renderDevice = nullptr;

        platform::destroy_window(m_primaryWindow);
        m_primaryWindow = nullptr;

        RUNE_CLIENT_INFO("Sandbox shutdown.");
    }

    void update(Engine& engine) override
    {
        m_renderSurface->acquire_next_image();

        m_cmdList->reset();
        m_cmdList->begin();

        m_cmdList->transition_state(m_renderSurface->current_image(), rhi::ResourceState::Undefined, rhi::ResourceState::RenderTarget);
        m_cmdList->transition_state(m_renderSurface->current_image(), rhi::ResourceState::RenderTarget, rhi::ResourceState::Present);

        m_cmdList->end();

        m_renderDevice->submit({ m_cmdList.get() }, m_fence.get(), ++m_fenceValue);
        m_fence->wait(m_fenceValue);

        m_renderSurface->present();

        if (m_primaryWindow->close_requested())
            engine.request_shutdown();
    }

private:
    platform::WindowPtr m_primaryWindow{};
    Shared<rhi::Device> m_renderDevice{};
    Owned<rhi::Surface> m_renderSurface{};
    Owned<rhi::CommandList> m_cmdList{};
    Owned<rhi::Fence> m_fence{};
    u64 m_fenceValue{};
};

int main()
{
    EngineConfig config{};
    config.logOutputDir = "./logs";

    Engine engine{};
    ApplicationSandbox app{};

    bool success = engine.run(&app, config);
    if (!success)
    {
        return 1;
    }

    return 0;
}