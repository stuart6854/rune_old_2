#pragma once

#include "internal_common.hpp"
#include "resources/metadata.hpp"
#include "resources/resource.hpp"
#include "graphics/renderer/renderer.hpp"

#include <rune/assetlib/mesh/static_mesh.hpp>

#include <memory>

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
}