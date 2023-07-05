#include "resources/system_resources.hpp"

#include "common_internal.hpp"
#include "resources/metadata.hpp"
#include "resources/resource.hpp"
#include "factory_funcs.hpp"

#include <yaml-cpp/yaml.h>

namespace rune
{
    void SystemResources::initialise()
    {
        register_factory(ResourceType::StaticMesh, factory_load_static_mesh);
        register_factory(ResourceType::Texture, factory_load_texture);
        register_factory(ResourceType::Material, factory_load_material);

        const std::filesystem::path dataDirPath = "../../data";
        LOG_INFO("resources - data dir: {}", std::filesystem::absolute(dataDirPath).string());
        RUNE_ASSERT(std::filesystem::exists(dataDirPath));

        const auto registryFile = dataDirPath / "registry.yaml";
        RUNE_ASSERT(std::filesystem::exists(registryFile));
        load_registry_file(registryFile);

        for (const auto& entry : std::filesystem::recursive_directory_iterator("../../data"))
        {
            if (!entry.is_regular_file())
            {
                continue;
            }

            const auto& filePath = entry.path();
            const auto& fileExt = filePath.extension();
            if (fileExt == ".bin")
            {
                register_from_disk(filePath);
            }
        }

        LOG_INFO("Resources system initialised.");
    }

    void SystemResources::update() {}

    void SystemResources::shutdown()
    {
        LOG_INFO("Resources system has shut down.");
    }

    void SystemResources::register_factory(ResourceType resourceType, FactoryFunc&& factoryFunc)
    {
        m_factoryFuncMap[resourceType] = factoryFunc;
    }

    void SystemResources::register_from_disk(const std::filesystem::path& filename)
    {
        const strid resourceId = STRID(filename.stem().string());
        auto& metadata = m_metadataMap[resourceId];
        metadata.type = ResourceType::StaticMesh;  // TODO: Handle different resource types
        metadata.source = ResourceSource::eDisk;
        metadata.state = ResourceState::NotLoaded;
        metadata.sourceFilename = filename;

        LOG_INFO("resources - Resource registered from disk: id={}, filename={}", resourceId, filename.string());
    }

    void SystemResources::add_resource(ResourceId id, std::unique_ptr<Resource>&& resource)
    {
        auto& metadata = m_metadataMap[id];
        metadata.state = ResourceState::Loaded;
        metadata.source = ResourceSource::eApp;
        metadata.resource = std::move(resource);
    }

    auto SystemResources::get_metadata(strid id) -> ResourceMetadata&
    {
        return m_metadataMap.at(id);
    }

    void SystemResources::load(ResourceId id)
    {
        auto& metadata = get_metadata(id);
        if (metadata.state != ResourceState::NotLoaded)
        {
            return;
        }

        for (auto depResourceId : metadata.dependencies)
        {
            load(depResourceId);
        }

        const auto& factoryFunc = get_factory(metadata.type);
        if (!factoryFunc)
        {
            LOG_ERROR("resources - No factory for resource type '_resource_type_'!");
            return;
        }

        metadata.state = ResourceState::Loading;
        metadata.resource = factoryFunc(*this, metadata);
        RUNE_ASSERT(metadata.resource != nullptr);

        metadata.state = ResourceState::Loaded;
    }

    void SystemResources::unload(ResourceId id)
    {
        auto& metadata = get_metadata(id);
        metadata.state = ResourceState::NotLoaded;
        metadata.resource = nullptr;
    }

    auto SystemResources::get_factory(ResourceType resourceType) -> const FactoryFunc&
    {
        return m_factoryFuncMap[resourceType];
    }

    void SystemResources::load_registry_file(const std::filesystem::path& filename)
    {
        auto registryFile = YAML::LoadFile(filename.string());
        RUNE_ASSERT(registryFile);

        auto resourcesNode = registryFile["resources"];
        RUNE_ASSERT(resourcesNode);

        for (auto resourceNode : resourcesNode)
        {
            auto idNode = resourceNode["id"];
            RUNE_ASSERT(idNode);
            const auto resourceId = idNode.as<u64>();

            auto& metadata = m_metadataMap[resourceId];

            metadata.type = ResourceType(resourceNode["type"].as<u8>());  // TODO: Handle different resource types
            metadata.source = ResourceSource::eDisk;
            metadata.state = ResourceState::NotLoaded;
            auto sourceFile = resourceNode["source"].as<std::string>();
            metadata.sourceFilename = filename.parent_path() / sourceFile;

            auto depsNode = resourceNode["deps"];
            for (auto&& depNode : depsNode)
            {
                metadata.dependencies.push_back(depNode.as<u64>());
            }

            LOG_INFO("resources - Resource registered from registry file: id={}, source={}", resourceId, metadata.sourceFilename.string());
        }
    }

}