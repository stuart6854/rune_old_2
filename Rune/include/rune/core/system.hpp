#pragma once

#include <type_traits>

namespace rune
{
    class ISystem
    {
    public:
        virtual ~ISystem() = default;

        virtual void initialise() = 0;

        virtual void update() = 0;

        virtual void shutdown() = 0;
    };

    template <class T>
    concept SystemType = std::is_base_of<ISystem, T>::value;
}