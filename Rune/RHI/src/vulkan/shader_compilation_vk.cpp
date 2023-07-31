#include "shader_compilation_vk.hpp"

#include <shaderc/shaderc.hpp>

#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>

namespace rune::rhi
{
    auto convert(vk::ShaderStageFlagBits stage) -> shaderc_shader_kind
    {
        switch (stage)
        {
            case vk::ShaderStageFlagBits::eVertex: return shaderc_vertex_shader;
            case vk::ShaderStageFlagBits::eTessellationControl: return shaderc_tess_control_shader;
            case vk::ShaderStageFlagBits::eTessellationEvaluation: return shaderc_tess_evaluation_shader;
            case vk::ShaderStageFlagBits::eGeometry: return shaderc_geometry_shader;
            case vk::ShaderStageFlagBits::eFragment: return shaderc_fragment_shader;
            case vk::ShaderStageFlagBits::eCompute: return shaderc_compute_shader;
        }
        assert(false);
        return {};
    }

    auto read_shader_source(const std::string& filename) -> std::string
    {
        std::ifstream file(filename, std::ios::in);
        if (!file)
            return "";

        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }

    auto compile_shader(const std::string& sourceCode,
                        vk::ShaderStageFlagBits stage,
                        bool debugInfo,
                        bool optimize,
                        const std::vector<std::string>& defines,
                        const char* filename) -> std::vector<std::uint8_t>
    {
        shaderc::CompileOptions options;
        for (auto& define : defines)
            options.AddMacroDefinition(define);

        if (debugInfo)
            options.SetGenerateDebugInfo();

        if (optimize)
            options.SetOptimizationLevel(shaderc_optimization_level_performance);

        shaderc::Compiler compiler;
        auto result = compiler.CompileGlslToSpv(sourceCode, convert(stage), filename, options);
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