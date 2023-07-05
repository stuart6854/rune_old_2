#pragma once

namespace rune
{
    using ResourceId = u64;

    struct Resource
    {
        virtual ~Resource() = default;
    };

}