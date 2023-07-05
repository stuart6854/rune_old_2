#include "factory_funcs.hpp"

#include "resources/system_resources.hpp"
#include "rendering/system_renderer.hpp"
#include "rendering/static_mesh.hpp"
#include "rendering/texture.hpp"
#include "rendering/material.hpp"

#include <rune/assetlib/mesh/static_mesh.hpp>

#include <yaml-cpp/yaml.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <vector>

namespace rune
{
    auto factory_load_static_mesh(SystemResources& resourceSystem, const ResourceMetadata& metadata) -> std::unique_ptr<Resource>
    {
        RUNE_UNUSED(resourceSystem);

        std::ifstream file(metadata.sourceFilename, std::ios::binary);
        assetlib::mesh::StaticMesh loadedMesh{};
        assetlib::mesh::import_static_mesh_optimised(file, loadedMesh);

        auto mesh = Engine::get().get_system<SystemRenderer>()->create_static_mesh();
        mesh->set_positions(loadedMesh.positions);
        mesh->set_normals(loadedMesh.normals);
        mesh->set_tex_coords(loadedMesh.texCoords);
        mesh->set_triangles(loadedMesh.triangles);
        mesh->apply();
        return mesh;
    }

    auto factory_load_texture(SystemResources& resourceSystem, const ResourceMetadata& metadata) -> std::unique_ptr<Resource>
    {
        RUNE_UNUSED(resourceSystem);

        i32 width{};
        i32 height{};
        i32 comps{};

        u8* data = stbi_load(metadata.sourceFilename.string().c_str(), &width, &height, &comps, 4);
        auto dataVec = std::vector(data, data + width * height * 4);

        auto texture = Engine::get().get_system<SystemRenderer>()->create_texture();
        texture->set_dimensions(width, height, 1);
        texture->set_data(dataVec);

        stbi_image_free(data);
        return texture;
    }

    auto factory_load_material(SystemResources& resourceSystem, const ResourceMetadata& metadata) -> std::unique_ptr<Resource>
    {
        auto materialFile = YAML::LoadFile(metadata.sourceFilename.string());
        RUNE_ASSERT(materialFile);

        auto materialNode = materialFile["material"];
        RUNE_ASSERT(materialNode);

        auto material = Engine::get().get_system<SystemRenderer>()->create_material();

        std::vector<ResourceHandle<Texture>> textures{};
        auto texturesNode = materialNode["textures"];
        for (auto&& textureNode : texturesNode)
        {
            //            auto textureName = textureNode["name"].as<std::string>();
            auto textureResourceId = textureNode["id"].as<u64>();
            textures.push_back(resourceSystem.get_ptr<Texture>(textureResourceId));
        }
        material->set_textures(textures);

        return material;
    }

}