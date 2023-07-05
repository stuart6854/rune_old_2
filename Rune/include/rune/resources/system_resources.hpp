#pragma once

#include "common.hpp"
#include "core/system.hpp"
#include "handle.hpp"
#include "types.hpp"

#include <memory>
#include <functional>
#include <unordered_map>
#include <unordered_set>

namespace rune
{
    class SystemResources : public ISystem
    {
    public:
        void initialise() override;
        void update() override;
        void shutdown() override;

        using FactoryFunc = std::function<std::unique_ptr<Resource>(SystemResources& resourceSystem, const ResourceMetadata&)>;
        void register_factory(ResourceType resourceType, FactoryFunc&& factoryFunc);

        void register_from_disk(const std::filesystem::path& filename);

        void add_resource(ResourceId id, std::unique_ptr<Resource>&& resource);

        auto get_metadata(ResourceId id) -> ResourceMetadata&;

        template <typename T>
        auto get_ptr(ResourceId id) -> ResourceHandle<T>;

        void load(ResourceId id);
        void unload(ResourceId id);

    private:
        auto get_factory(ResourceType resourceType) -> const FactoryFunc&;
        void load_registry_file(const std::filesystem::path& filename);

    private:
        std::unordered_map<ResourceType, FactoryFunc> m_factoryFuncMap{};
        std::unordered_map<ResourceId, ResourceMetadata> m_metadataMap{};
        std::unordered_set<strid> m_loadedResourcesSet{};
    };

    template <typename T>
    auto SystemResources::get_ptr(ResourceId id) -> ResourceHandle<T>
    {
        auto& metadata = get_metadata(id);
        return ResourceHandle<T>(&metadata);
    }

}