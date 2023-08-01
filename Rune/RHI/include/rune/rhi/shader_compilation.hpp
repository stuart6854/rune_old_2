#pragma once

#include "enums.hpp"

#include <vector>
#include <cstdint>
#include <string>

namespace rune::rhi
{
    enum class ShaderSourceLanguage
    {
        GLSL,
        HLSL,
    };

    enum class ShaderRuntimeLanguage
    {
        SPIRV,
    };

    struct ShaderCompileDesc
    {
        ShaderSourceLanguage sourceLanguage;
        std::string sourceCode;
        std::string sourceFilename;
        ShaderStages stage;

        bool debug = false;
        bool optimize = true;
        std::vector<std::string> defines;
    };

    auto compile_shader_from_source(const ShaderCompileDesc& desc) -> std::vector<std::uint8_t>;
}