#include "mesh/static_mesh.hpp"

#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <iostream>

#define READ(_varType, _varName) stream.read(reinterpret_cast<char*>(&_varName), sizeof(_varType));
#define WRITE(_varName, _type) stream.write((char*)&_varName, sizeof(_type))

namespace rune::assetlib::mesh
{
    constexpr auto ASSIMP_BASE_IMPORT_FLAGS = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_MaxQuality;  // NOLINT

    namespace
    {
        void assimp_process_mesh(StaticMesh& outMesh, aiMesh* mesh, const aiScene* /*scene*/)
        {
            auto& submesh = outMesh.submeshes.emplace_back();
            submesh.indexOffset = std::uint32_t(outMesh.triangles.size());
            submesh.indexCount = std::uint32_t(mesh->mNumFaces * 3);

            outMesh.positions.resize(outMesh.positions.size() + mesh->mNumVertices);
            outMesh.normals.resize(outMesh.normals.size() + mesh->mNumVertices);
            outMesh.texCoords.resize(outMesh.texCoords.size() + mesh->mNumVertices);
            outMesh.triangles.resize(outMesh.triangles.size() + mesh->mNumFaces * 3);

            for (auto i = 0u; i < mesh->mNumVertices; ++i)
            {
                auto& position = outMesh.positions[i];
                position.x = mesh->mVertices[i].x;
                position.y = mesh->mVertices[i].y;
                position.z = mesh->mVertices[i].z;

                if (mesh->HasNormals())
                {
                    auto& normal = outMesh.normals[i];
                    normal.x = mesh->mNormals[i].x;
                    normal.y = mesh->mNormals[i].y;
                    normal.z = mesh->mNormals[i].z;
                }
                else
                {
                    outMesh.normals[i] = {};
                }

                if (mesh->HasTextureCoords(0))
                {
                    auto& texCoord = outMesh.texCoords[i];
                    texCoord.x = mesh->mTextureCoords[0][i].x;
                    texCoord.y = mesh->mTextureCoords[0][i].y;
                }
                else
                {
                    outMesh.texCoords[i] = {};
                }
            }

            for (auto i = 0u; i < mesh->mNumFaces; ++i)
            {
                const auto& face = mesh->mFaces[i];
                for (auto j = 0u; j < face.mNumIndices; ++j)
                {
                    outMesh.triangles[i * 3 + j] = face.mIndices[j];
                }
            }

            // #TODO: process material
        }

        void assimp_process_node(aiNode* node, const aiScene* scene, std::vector<StaticMesh>& outMeshes)
        {
            for (auto i = 0u; i < node->mNumMeshes; ++i)
            {
                auto* assimpMesh = scene->mMeshes[node->mMeshes[i]];
                auto& outMesh = outMeshes.emplace_back();
                assimp_process_mesh(outMesh, assimpMesh, scene);
            }
            for (auto i = 0u; i < node->mNumChildren; ++i)
            {
                assimp_process_node(node->mChildren[i], scene, outMeshes);
            }
        }
    }

    auto import_static_mesh_raw(const std::filesystem::path& filename, RawImportSettings importSettings) -> std::vector<StaticMesh>
    {
        const auto fileExt = filename.extension().string();
        if (!aiIsExtensionSupported(fileExt.c_str()))
        {
            // #TODO: Print error
            return {};
        }

        auto importFlags = ASSIMP_BASE_IMPORT_FLAGS;
        if (importSettings.separateMeshes)
        {
            importFlags |= aiProcess_PreTransformVertices;
        }

        Assimp::Importer importer{};
        const auto* scene = importer.ReadFile(filename.string(), importFlags);
        if (scene == nullptr)
        {
            std::cout << importer.GetErrorString() << std::endl;
            return {};
        }

        std::vector<StaticMesh> meshes{};
        assimp_process_node(scene->mRootNode, scene, meshes);

        return meshes;
    }

    void import_static_mesh_optimised(std::ifstream& stream, StaticMesh& outMesh)
    {
        if (!stream)
        {
            return;
        }

        std::uint64_t positionCount{};
        READ(std::uint64_t, positionCount);
        outMesh.positions.resize(positionCount);
        for (auto i = 0u; i < positionCount; ++i)
        {
            auto& position = outMesh.positions[i];
            READ(float, position.x);
            READ(float, position.y);
            READ(float, position.z);
        }

        std::uint64_t normalCount{};
        READ(std::uint64_t, normalCount);
        outMesh.normals.resize(normalCount);
        for (auto i = 0u; i < normalCount; ++i)
        {
            auto& normal = outMesh.normals[i];
            READ(float, normal.x);
            READ(float, normal.y);
            READ(float, normal.z);
        }

        std::uint64_t texCoordCount{};
        READ(std::uint64_t, texCoordCount);
        outMesh.texCoords.resize(texCoordCount);
        for (auto i = 0u; i < texCoordCount; ++i)
        {
            auto& texCoord = outMesh.texCoords[i];
            READ(float, texCoord.x);
            READ(float, texCoord.y);
        }

        std::uint64_t triangleCount{};
        READ(std::uint64_t, triangleCount);
        outMesh.triangles.resize(triangleCount);
        for (auto i = 0u; i < triangleCount; ++i)
        {
            READ(std::uint32_t, outMesh.triangles[i]);
        }

        std::uint8_t submeshCount{};
        READ(std::uint8_t, submeshCount);
        outMesh.submeshes.resize(submeshCount);
        for (auto i = 0u; i < submeshCount; ++i)
        {
            auto& sub = outMesh.submeshes[i];
            READ(std::uint32_t, sub.indexOffset);
            READ(std::uint32_t, sub.indexCount);
        }
    }

    void export_static_mesh_optimised(std::ofstream& stream, const StaticMesh& mesh)
    {
        auto positionCount = std::uint64_t(mesh.positions.size());
        WRITE(positionCount, std::uint64_t);
        for (const auto& position : mesh.positions)
        {
            WRITE(position.x, float);
            WRITE(position.y, float);
            WRITE(position.z, float);
        }

        auto normalCount = std::uint64_t(mesh.normals.size());
        WRITE(normalCount, std::uint64_t);
        for (const auto& normal : mesh.normals)
        {
            WRITE(normal.x, float);
            WRITE(normal.y, float);
            WRITE(normal.z, float);
        }

        auto texCoordCount = std::uint64_t(mesh.texCoords.size());
        WRITE(texCoordCount, std::uint64_t);
        for (const auto& texCoord : mesh.texCoords)
        {
            WRITE(texCoord.x, float);
            WRITE(texCoord.y, float);
        }

        auto triangleCount = std::uint64_t(mesh.triangles.size());
        WRITE(triangleCount, std::uint64_t);
        for (const auto& triangle : mesh.triangles)
        {
            WRITE(triangle, std::uint32_t);
        }

        auto submeshCount = std::uint8_t(mesh.submeshes.size());
        WRITE(submeshCount, std::uint8_t);
        for (const auto& submesh : mesh.submeshes)
        {
            WRITE(submesh.indexOffset, std::uint32_t);
            WRITE(submesh.indexCount, std::uint32_t);
        }
    }

}