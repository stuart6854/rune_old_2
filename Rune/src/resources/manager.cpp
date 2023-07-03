#include "resources/manager.hpp"

#include "internal_common.hpp"
#include "resources/metadata.hpp"
#include "resources/resource.hpp"

#include <unordered_set>
#include <unordered_map>

namespace rune::resources
{
    struct ManagerData
    {
        std::unordered_map<ResourceId, Metadata> metadataMap{};
        std::unordered_set<strid> loadedResourcesSet{};
    };
    std::unique_ptr<ManagerData> g_managerData{ nullptr };  // NOLINT

    void initialise()
    {
        RUNE_ASSERT(g_managerData == nullptr);
        g_managerData = std::make_unique<ManagerData>();
    }

    void shutdown()
    {
        RUNE_ASSERT(g_managerData != nullptr);
        g_managerData = nullptr;
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

}