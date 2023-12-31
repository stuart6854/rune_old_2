#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string_view>

#define GLM_FORCE_XYZW_ONLY

namespace rune
{
    using f32 = float;
    using f64 = double;

    using i8 = std::int8_t;
    using i16 = std::int16_t;
    using i32 = std::int32_t;
    using i64 = std::int64_t;

    using u8 = std::uint8_t;
    using u16 = std::uint16_t;
    using u32 = std::uint32_t;
    using u64 = std::uint64_t;
    using sizet = std::size_t;

    // # TODO: Replace with StringId type
    using strid = sizet;
#define STRID(_str) std::hash<std::string_view>{}(_str)

}