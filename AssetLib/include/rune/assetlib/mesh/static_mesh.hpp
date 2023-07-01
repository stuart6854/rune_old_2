#pragma once

#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>

#include <vector>
#include <fstream>
#include <filesystem>

namespace rune::assetlib::mesh
{
    struct Submesh
    {
        std::uint32_t indexOffset;
        std::uint32_t indexCount;
    };

    struct StaticMesh
    {
        std::vector<glm::vec3> positions{};
        std::vector<glm::vec3> normals{};
        std::vector<glm::vec2> texCoords{};
        std::vector<std::uint32_t> triangles{};
        std::vector<Submesh> submeshes{};
    };

    struct RawImportSettings
    {
        bool separateMeshes{ false };
    };
    auto import_static_mesh_raw(const std::filesystem::path& filename, RawImportSettings importSettings = {}) -> std::vector<StaticMesh>;
    void import_static_mesh_optimised(std::ifstream& stream, StaticMesh& outMesh);

    void export_static_mesh_optimised(std::ofstream& stream, const StaticMesh& mesh);
}