/*
 * https://voithos.io/articles/enum-class-bitmasks/
 */

#pragma once

#include <type_traits>

#define RUNE_DEFINE_ENUM_CLASS_BITWISE_OPERATORS(EnumType)                                      \
    inline constexpr EnumType operator|(EnumType Lhs, EnumType Rhs)                             \
    {                                                                                           \
        return static_cast<EnumType>(static_cast<std::underlying_type_t<EnumType>>(Lhs) |       \
                                     static_cast<std::underlying_type_t<EnumType>>(Rhs));       \
    }                                                                                           \
    inline constexpr EnumType operator&(EnumType Lhs, EnumType Rhs)                             \
    {                                                                                           \
        return static_cast<EnumType>(static_cast<std::underlying_type_t<EnumType>>(Lhs) &       \
                                     static_cast<std::underlying_type_t<EnumType>>(Rhs));       \
    }                                                                                           \
    inline constexpr EnumType operator^(EnumType Lhs, EnumType Rhs)                             \
    {                                                                                           \
        return static_cast<EnumType>(static_cast<std::underlying_type_t<EnumType>>(Lhs) ^       \
                                     static_cast<std::underlying_type_t<EnumType>>(Rhs));       \
    }                                                                                           \
    inline constexpr EnumType operator~(EnumType E)                                             \
    {                                                                                           \
        return static_cast<EnumType>(~static_cast<std::underlying_type_t<EnumType>>(E));        \
    }                                                                                           \
    inline EnumType& operator|=(EnumType& Lhs, EnumType Rhs)                                    \
    {                                                                                           \
        return Lhs = static_cast<EnumType>(static_cast<std::underlying_type_t<EnumType>>(Lhs) | \
                                           static_cast<std::underlying_type_t<EnumType>>(Lhs)); \
    }                                                                                           \
    inline EnumType& operator&=(EnumType& Lhs, EnumType Rhs)                                    \
    {                                                                                           \
        return Lhs = static_cast<EnumType>(static_cast<std::underlying_type_t<EnumType>>(Lhs) & \
                                           static_cast<std::underlying_type_t<EnumType>>(Lhs)); \
    }                                                                                           \
    inline EnumType& operator^=(EnumType& Lhs, EnumType Rhs)                                    \
    {                                                                                           \
        return Lhs = static_cast<EnumType>(static_cast<std::underlying_type_t<EnumType>>(Lhs) ^ \
                                           static_cast<std::underlying_type_t<EnumType>>(Lhs)); \
    }
