#include <rune/rune.hpp>

int main()
{
    using namespace rune;

    EngineConfig config{};

    Engine engine{};

    bool success = engine.run(config);
    if (!success)
    {
        return 1;
    }

    return 0;
}