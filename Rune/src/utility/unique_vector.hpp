#pragma once

#include "common.hpp"

#include <vector>

namespace rune::utility
{
    template <typename T>
    class UniqueVector
    {
    public:
        UniqueVector() = default;
        UniqueVector(sizet size);
        UniqueVector(sizet size, const T& initialValue);
        ~UniqueVector();

        void reserve(sizet size);
        void resize(sizet size);

        auto insert(const T& value) -> sizet;

        auto at(sizet index) -> T&;

        auto operator[](sizet index) -> T&;

    private:
        sizet m_size{};
        sizet m_capacity{};
    };
}