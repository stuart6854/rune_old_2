#pragma once

namespace rune
{
    class Engine;

    class IApplication
    {
    public:
        virtual ~IApplication() = default;

        virtual void initialise(Engine& engine) = 0;
        virtual void shutdown(Engine& engine) = 0;

        virtual void update(Engine& engine) = 0;
    };
}