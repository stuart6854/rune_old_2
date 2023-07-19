#include <rune/rune.hpp>

using namespace rune;

class ApplicationSandbox : public IApplication
{
public:
    void initialise(Engine& /*engine*/) override
    {
        m_primaryWindow = platform::create_window("Sandbox", 800, 600);

        RUNE_CLIENT_INFO("Sandbox initialised.");
    }

    void shutdown(Engine& /*engine*/) override
    {
        platform::destroy_window(m_primaryWindow);
        m_primaryWindow = nullptr;

        RUNE_CLIENT_INFO("Sandbox shutdown.");
    }

    void update(Engine& engine) override
    {
        if (m_primaryWindow->close_requested())
            engine.request_shutdown();
    }

private:
    platform::WindowPtr m_primaryWindow{};
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