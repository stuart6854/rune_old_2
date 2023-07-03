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

        const std::filesystem::path dataDirPath = "../../data";
        LOG_INFO("resources - data dir: {}", std::filesystem::absolute(dataDirPath).string());
        RUNE_ASSERT(std::filesystem::exists(dataDirPath));

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

    void register_from_disk(const std::filesystem::path& filename)
    {
        const strid resourceId = STRID(filename.stem().string());
        auto& metadata = g_managerData->metadataMap[resourceId];
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

}