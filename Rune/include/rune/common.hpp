#pragma once

#include <memory>
#include <cstdint>
#include <cstddef>
#include <functional>

namespace rune
{
    using i8 = std::int8_t;
    using i16 = std::int16_t;
    using i32 = std::int32_t;
    using i64 = std::int64_t;

    using u8 = std::uint8_t;
    using u16 = std::uint16_t;
    using u32 = std::uint32_t;
    using u64 = std::uint64_t;

    using sizet = std::size_t;

    using f32 = float;
    using f64 = double;

    template <typename T>
    using Owned = std::unique_ptr<T>;

    template <typename T>
    using Shared = std::shared_ptr<T>;

    template <typename T, typename... Args>
    auto create_owned(Args&&... args) -> Owned<T>
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template <typename T, typename... Args>
    auto create_shared(Args&&... args) -> Shared<T>
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    using hasht = u64;

    template <typename T>
    constexpr auto combine_hash(hasht& seed, const T& v)
    {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

}

#define RUNE_UNUSED(_x) (void)(_x)

#if defined(RUNE_DEBUG) || defined(RUNE_RELEASE)
    #ifdef RUNE_PLATFORM_WINDOWS
        #define RUNE_DEBUG_BREAK() __debugbreak()
    #endif
#else
    #define RUNE_DEBUG_BREAK()
#endif