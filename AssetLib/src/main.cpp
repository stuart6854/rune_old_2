#include <rune/assetlib/mesh/static_mesh.hpp>
#include <cxxopts.hpp>

#include <string>
#include <format>
#include <fstream>
#include <iostream>

using namespace rune::assetlib;

#define PRINT(...)                                              \
    do                                                          \
    {                                                           \
        if (!quietPrint)                                        \
        {                                                       \
            std::cout << std::format(__VA_ARGS__) << std::endl; \
        }                                                       \
    } while (false)

#define VERBOSE_PRINT(...)      \
    do                          \
    {                           \
        if (verbosePrint)       \
        {                       \
            PRINT(__VA_ARGS__); \
        }                       \
    } while (false)

std::vector<mesh::StaticMesh> importedMeshesStatic{};

int main(int argc, char** argv)
{
    cxxopts::Options options("Rune AssetLib Command Line", "Rune AssetLib - Rune Engine Asset Import/Export Tools");
    // clang-format off
    options.add_options("")
        ("b, batch", "Batch export all imported files into single bank")
        ("o,output", "Output filename/directory", cxxopts::value<std::string>()->default_value(""))
        ("q,quiet", "Only output errors")
        ("v,verbose", "Verbose output");
    options.add_options("Mesh")
        ("t,type", "Force mesh type ('static', 'skeletal')")
        ("s,separate", "Separate submeshes into separate meshes");
    // clang-format on

    options.help({ "", "Mesh" });

    auto result = options.parse(argc, argv);

    // Any input files?
    const auto& inputFiles = result.unmatched();
    if (inputFiles.empty())
    {
        std::cout << options.help() << std::endl;
        return 0;
    }
    const bool quietPrint = result["quiet"].as<bool>();
    const bool verbosePrint = result["verbose"].as<bool>();
    const bool batchExport = result["batch"].as<bool>();

    for (const auto& inputFile : inputFiles)
    {
        PRINT("Importing => {}", inputFile);
        auto importedMeshes = mesh::import_static_mesh_raw(inputFile);
        PRINT("Imported {} separate meshes", importedMeshes.size());
        importedMeshesStatic.insert(importedMeshesStatic.end(), importedMeshes.begin(), importedMeshes.end());

        if (verbosePrint)
        {
            for (const auto& mesh : importedMeshes)
            {
                VERBOSE_PRINT(" - Mesh: {} vertices, {} triangles, {} submeshs",
                              mesh.positions.size(),
                              mesh.triangles.size() / 3,
                              mesh.submeshes.size());
            }
        }

        if (!batchExport)
        {
            int i = 0;
            for (const auto& mesh : importedMeshes)
            {
                const auto exportFilename = std::filesystem::path(inputFile).stem().string() + "_" + std::to_string(i) + ".bin";
                std::ofstream file(exportFilename, std::ios::binary);
                mesh::export_static_mesh_optimised(file, mesh);
            }
        }
    }

    if (batchExport)
    {
        // #TODO: Export all assets batched into single bank
    }
}