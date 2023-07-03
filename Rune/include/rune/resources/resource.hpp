#pragma once

namespace rune::resources
{
    using ResourceId = u64;

    struct Resource
    {
        virtual ~Resource() = default;
    };

}