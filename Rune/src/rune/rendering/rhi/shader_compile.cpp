#include "rune/rendering/rhi/shader_compile.hpp"

#include "rune/debug/log_engine.hpp"
#include "rune/debug/assert_engine.hpp"

#include <shaderc/shaderc.hpp>

#include <fstream>
#include <sstream>

namespace rune::rhi
{
    namespace
    {
        auto read_source_file(const std::filesystem::path& filename) -> std::string
        {
            if (!std::filesystem::is_regular_file(filename))
            {
                RUNE_ENG_ERROR("Shader source file does not exist: {}", filename.string());
                return "";
            }

            std::fstream file(filename);
            if (!file)
            {
                RUNE_ENG_ERROR("Failed to open shader source file: {}", filename.string());
                return "";
            }

            std::stringstream buffer;
            buffer << file.rdbuf();
            return buffer.str();
        }

        auto convert_to_shaderc(ShaderStage stage) -> shaderc_shader_kind
        {
            switch (stage)
            {
                case rune::rhi::ShaderStage::Vertex: return shaderc_vertex_shader;
                case rune::rhi::ShaderStage::Hull: return shaderc_tess_control_shader;
                case rune::rhi::ShaderStage::Domain: return shaderc_tess_evaluation_shader;
                case rune::rhi::ShaderStage::Geometry: return shaderc_geometry_shader;
                case rune::rhi::ShaderStage::Pixel: return shaderc_fragment_shader;
                case rune::rhi::ShaderStage::Compute: return shaderc_compute_shader;
            }

            RUNE_ENG_ERROR("Unknown ShaderStage.");
            RUNE_ENG_ASSERT(false);
            return {};
        }

        auto compile_glsl_to_spirv(const ShaderCompileDecl& decl) -> std::vector<u8>
        {
            const auto& source = decl.Source;
            auto shaderKind = convert_to_shaderc(decl.Stage);
            const auto sourceFilename = decl.SourceFilename.string();
            const auto& entryPointName = decl.EntryPointName;

            shaderc::CompileOptions options;
            if (decl.GenerateDebugInfo)
                options.SetGenerateDebugInfo();
            if (decl.Optimize)
                options.SetOptimizationLevel(shaderc_optimization_level_performance);
            for (const auto& define : decl.Defines)
                options.AddMacroDefinition(define);

            shaderc::Compiler compiler;
            auto result = compiler.CompileGlslToSpv(source, shaderKind, sourceFilename.c_str(), entryPointName.c_str(), options);
            if (result.GetCompilationStatus() != shaderc_compilation_status_success)
            {
                RUNE_ENG_ERROR("Shader Compile Error: {}", result.GetErrorMessage());
                return {};
            }

            std::vector spirvCode(result.cbegin(), result.cend());
            std::vector<u8> byteCode(spirvCode.size() * sizeof(u32));
            std::memcpy(byteCode.data(), spirvCode.data(), byteCode.size());

            return byteCode;
        }
    }

    auto compile_shader(ShaderCompileDecl& decl) -> std::vector<u8>
    {
        if (decl.Source.empty())
        {
            if (decl.SourceFilename.empty())
            {
                RUNE_ENG_ERROR("No shader Source or SourceFilename to compile.");
                return {};
            }

            decl.Source = read_source_file(decl.SourceFilename);
            if (decl.Source.empty())
                return {};
        }

        if (decl.SourceLanguage == ShaderSourceLanguage::GLSL)
        {
            if (decl.OutputRuntimeLanguage == ShaderRuntimeLanguage::SPIRV)
            {
                return compile_glsl_to_spirv(decl);
            }
        }

        RUNE_ENG_ERROR("Unable to compile shader.");
        RUNE_ENG_ASSERT(false);
        return {};
    }

}