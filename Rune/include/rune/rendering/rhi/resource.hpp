#pragma once

namespace rune::rhi
{
    class Resource
    {
    public:
        virtual ~Resource() = default;

        // #TODO: set_debug_name(const std:string& name);
    };
}