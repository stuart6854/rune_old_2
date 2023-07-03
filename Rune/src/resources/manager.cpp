#include "resources/manager.hpp"

#include "internal_common.hpp"
#include "resources/metadata.hpp"
#include "resources/resource.hpp"
#include "factory_funcs.hpp"

#include <yaml-cpp/yaml.h>

#include <unordered_set>
#include <unordered_map>

namespace rune::resources
{
    struct ManagerData
    {
        std::unordered_map<ResourceType, FactoryFunc> factoryFuncMap{};
        std::unordered_map<ResourceId, Metadata> metadataMap{};
        std::unordered_set<strid> loadedResourcesSet{};
    };
    std::unique_ptr<ManagerData> g_managerData{ nullptr };  // NOLINT

    namespace
    {
        auto get_factory(ResourceType resourceType) -> const FactoryFunc&
        {
            RUNE_ASSERT(g_managerData != nullptr);
            return g_managerData->factoryFuncMap[resourceType];
        }

        void load_registry_file(const std::filesystem::path& filename)
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

                auto& metadata = g_managerData->metadataMap[resourceId];

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

                LOG_INFO(
                    "resources - Resource registered from registry file: id={}, source={}", resourceId, metadata.sourceFilename.string());
            }
        }
    }

    void initialise()
    {
        RUNE_ASSERT(g_managerData == nullptr);
        g_managerData = std::make_unique<ManagerData>();

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
    }

    void shutdown()
    {
        RUNE_ASSERT(g_managerData != nullptr);
        g_managerData = nullptr;
    }

    void register_factory(ResourceType resourceType, FactoryFunc&& factoryFunc)
    {
        RUNE_ASSERT(g_managerData != nullptr);
        g_managerData->factoryFuncMap[resourceType] = factoryFunc;
    }

    void register_from_disk(const std::filesystem::path& filename)
    {
        const strid resourceId = STRID(filename.stem().string());
        auto& metadata = g_managerData->metadataMap[resourceId];
        metadata.type = ResourceType::StaticMesh;  // TODO: Handle different resource types
        metadata.source = ResourceSource::eDisk;
        metadata.state = ResourceState::NotLoaded;
        metadata.sourceFilename = filename;

        LOG_INFO("resources - Resource registered from disk: id={}, filename={}", resourceId, filename.string());
    }

    void add_resource(ResourceId id, std::unique_ptr<Resource>&& resource)
    {
        auto& metadata = g_managerData->metadataMap[id];
        metadata.state = ResourceState::Loaded;
        metadata.source = ResourceSource::eApp;
        metadata.resource = std::move(resource);
    }

    auto get_metadata(strid id) -> Metadata&
    {
        RUNE_ASSERT(g_managerData != nullptr);
        return g_managerData->metadataMap.at(id);
    }

    void load(ResourceId id)
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

        const auto& factory = get_factory(metadata.type);
        if (!factory)
        {
            LOG_ERROR("resources - No factory for resource type '_resource_type_'!");
            return;
        }

        metadata.state = ResourceState::Loading;
        metadata.resource = factory(metadata);
        RUNE_ASSERT(metadata.resource != nullptr);

        metadata.state = ResourceState::Loaded;
    }

    void unload(ResourceId id)
    {
        auto& metadata = get_metadata(id);
        metadata.state = ResourceState::NotLoaded;
        metadata.resource = nullptr;
    }

}