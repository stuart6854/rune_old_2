#include "factory_funcs.hpp"

#include "resources/manager.hpp"
#include "graphics/renderer/renderer.hpp"

#include <rune/assetlib/mesh/static_mesh.hpp>

#include <yaml-cpp/yaml.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <vector>

namespace rune::resources
{
    auto factory_load_static_mesh(const Metadata& metadata) -> std::unique_ptr<Resource>
    {
        std::ifstream file(metadata.sourceFilename, std::ios::binary);
        assetlib::mesh::StaticMesh loadedMesh{};
        assetlib::mesh::import_static_mesh_optimised(file, loadedMesh);

        auto mesh = graphics::renderer::create_static_mesh();
        mesh->set_positions(loadedMesh.positions);
        mesh->set_normals(loadedMesh.normals);
        mesh->set_tex_coords(loadedMesh.texCoords);
        mesh->set_triangles(loadedMesh.triangles);
        mesh->apply();
        return mesh;
    }

    auto factory_load_texture(const Metadata& metadata) -> std::unique_ptr<Resource>
    {
        i32 width{};
        i32 height{};
        i32 comps{};

        u8* data = stbi_load(metadata.sourceFilename.string().c_str(), &width, &height, &comps, 4);
        auto dataVec = std::vector(data, data + width * height * 4);

        auto texture = graphics::renderer::create_texture();
        texture->set_dimensions(width, height, 1);
        texture->set_data(dataVec);

        stbi_image_free(data);
        return texture;
    }

    auto factory_load_material(const Metadata& metadata) -> std::unique_ptr<Resource>
    {
        auto materialFile = YAML::LoadFile(metadata.sourceFilename.string());
        RUNE_ASSERT(materialFile);

        auto materialNode = materialFile["material"];
        RUNE_ASSERT(materialNode);

        auto material = graphics::renderer::create_material();

        std::vector<ResourceHandle<graphics::renderer::Texture>> textures{};
        auto texturesNode = materialNode["textures"];
        for (auto&& textureNode : texturesNode)
        {
            //            auto textureName = textureNode["name"].as<std::string>();
            auto textureResourceId = textureNode["id"].as<u64>();
            textures.push_back(get_ptr<graphics::renderer::Texture>(textureResourceId));
        }
        material->set_textures(textures);

        return material;
    }

}