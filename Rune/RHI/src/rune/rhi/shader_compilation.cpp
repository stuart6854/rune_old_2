#include "rune/rhi/shader_compilation.hpp"

#include <shaderc/shaderc.hpp>

#include <cassert>
#include <sstream>
#include <fstream>
#include <iostream>

namespace rune::rhi
{
    namespace
    {
        auto read_shader_source(const std::string& filename) -> std::string
        {
            std::ifstream file(filename, std::ios::in);
            if (!file)
                return "";

            std::stringstream ss;
            ss << file.rdbuf();
            return ss.str();
        }

        auto convert(ShaderSourceLanguage language) -> shaderc_source_language
        {
            switch (language)
            {
                case ShaderSourceLanguage::GLSL: return shaderc_source_language_glsl;
                case ShaderSourceLanguage::HLSL: return shaderc_source_language_hlsl;
            }
            assert(false);
            return {};
        }

        auto convert(ShaderStages stage) -> shaderc_shader_kind
        {
            switch (stage)
            {
                case ShaderStages::Vertex: return shaderc_vertex_shader;
                case ShaderStages::TessellationControl: return shaderc_tess_control_shader;
                case ShaderStages::TessellationEvaluation: return shaderc_tess_evaluation_shader;
                case ShaderStages::Geometry: return shaderc_geometry_shader;
                case ShaderStages::Fragment: return shaderc_fragment_shader;
                case ShaderStages::Compute: return shaderc_compute_shader;
            }
            assert(false);
            return {};
        }

    }

    auto compile_shader_from_source(const ShaderCompileDesc& desc) -> std::vector<std::uint8_t>
    {
        std::string sourceCode = desc.sourceCode;
        if (sourceCode.empty())
        {
            if (desc.sourceFilename.empty())
                return {};

            sourceCode = read_shader_source(desc.sourceFilename);
        }

        shaderc::CompileOptions options;
        options.SetSourceLanguage(convert(desc.sourceLanguage));

        for (auto& define : desc.defines)
            options.AddMacroDefinition(define);

        if (desc.debug)
            options.SetGenerateDebugInfo();

        if (desc.optimize)
            options.SetOptimizationLevel(shaderc_optimization_level_performance);

        shaderc::Compiler compiler;
        auto result = compiler.CompileGlslToSpv(sourceCode, convert(desc.stage), desc.sourceFilename.c_str(), options);
        if (result.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            std::cerr << result.GetErrorMessage() << std::endl;
            return {};
        }

        std::vector<std::uint32_t> spirv(result.cbegin(), result.cend());
        std::vector<std::uint8_t> byteCode(spirv.size() * sizeof(std::uint32_t));
        std::memcpy(byteCode.data(), spirv.data(), byteCode.size());
        return byteCode;
    }
}