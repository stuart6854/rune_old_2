#pragma once

#include "rune/common.hpp"
#include "shader_program.hpp"

#include <string>
#include <vector>
#include <filesystem>

namespace rune::rhi
{
    struct ShaderCompileDecl
    {
        ShaderSourceLanguage SourceLanguage{};
        std::string Source{};
        std::filesystem::path SourceFilename{};  // Used to load source if `Source` is empty
        ShaderStage Stage{};
        std::string EntryPointName{};

        ShaderRuntimeLanguage OutputRuntimeLanguage{};

        bool GenerateDebugInfo{};
        bool Optimize{};
        std::vector<std::string> Defines{};
    };

    auto compile_shader(ShaderCompileDecl& decl) -> std::vector<u8>;
}