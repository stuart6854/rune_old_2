#pragma once

#include <type_traits>

namespace rune::rhi
{
#define DEFINE_ENUM_CLASS_BITWISE_OPERATORS(Enum)                                                                                        \
    inline constexpr Enum operator|(Enum Lhs, Enum Rhs)                                                                                  \
    {                                                                                                                                    \
        return static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(Lhs) | static_cast<std::underlying_type_t<Enum>>(Rhs));       \
    }                                                                                                                                    \
    inline constexpr Enum operator&(Enum Lhs, Enum Rhs)                                                                                  \
    {                                                                                                                                    \
        return static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(Lhs) & static_cast<std::underlying_type_t<Enum>>(Rhs));       \
    }                                                                                                                                    \
    inline constexpr Enum operator^(Enum Lhs, Enum Rhs)                                                                                  \
    {                                                                                                                                    \
        return static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(Lhs) ^ static_cast<std::underlying_type_t<Enum>>(Rhs));       \
    }                                                                                                                                    \
    inline constexpr Enum operator~(Enum E)                                                                                              \
    {                                                                                                                                    \
        return static_cast<Enum>(~static_cast<std::underlying_type_t<Enum>>(E));                                                         \
    }                                                                                                                                    \
    inline Enum& operator|=(Enum& Lhs, Enum Rhs)                                                                                         \
    {                                                                                                                                    \
        return Lhs = static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(Lhs) | static_cast<std::underlying_type_t<Enum>>(Lhs)); \
    }                                                                                                                                    \
    inline Enum& operator&=(Enum& Lhs, Enum Rhs)                                                                                         \
    {                                                                                                                                    \
        return Lhs = static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(Lhs) & static_cast<std::underlying_type_t<Enum>>(Lhs)); \
    }                                                                                                                                    \
    inline Enum& operator^=(Enum& Lhs, Enum Rhs)                                                                                         \
    {                                                                                                                                    \
        return Lhs = static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(Lhs) ^ static_cast<std::underlying_type_t<Enum>>(Lhs)); \
    }

}