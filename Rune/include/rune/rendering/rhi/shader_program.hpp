#pragma once

#include "rune/common.hpp"

#include <array>
#include <string>
#include <vector>

namespace rune::rhi
{
    enum class ShaderSourceLanguage
    {
        GLSL,
        // HLSL,
    };

    enum class ShaderRuntimeLanguage
    {
        SPIRV,
        // DXIL, // D3D bytecode
    };

    enum class ShaderType : u8
    {
        Vertex,
        Hull,
        Domain,
        Geometry,
        Pixel,
        Compute,

        Count
    };

    enum class ShaderStage
    {
        Vertex = (1 << 0),
        Hull = (1 << 1),    // Tessellation Control
        Domain = (1 << 2),  // Tessellation Evaluation
        Geometry = (1 << 3),
        Pixel = (1 << 4),  // Fragment
        Compute = (1 << 5),

        All = Vertex | Hull | Domain | Geometry | Pixel | Compute
    };

    struct ShaderDecl
    {
        std::string SourceFilePath;
        std::string SourceCode{};
        std::string SourceEntryPoint;

        std::vector<u8> ByteCode{};
    };

    struct ShaderProgramDecl
    {
        std::array<ShaderDecl, u8(ShaderType::Count)> ShaderStages{};
        // Vertex Format
    };

    class Device;

    /* A ShaderProgram is defined by shader stages that make a pipeline. */
    class ShaderProgram
    {
    public:
        static auto create(Shared<Device> device, const ShaderProgramDecl& decl) -> Owned<ShaderProgram>;

        virtual ~ShaderProgram() = default;
    };
}