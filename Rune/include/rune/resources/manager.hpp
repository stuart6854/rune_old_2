#pragma once

#include "common.hpp"
#include "handle.hpp"
#include "types.hpp"

#include <memory>
#include <functional>

namespace rune::resources
{
    void initialise();
    void shutdown();

    using FactoryFunc = std::function<std::unique_ptr<Resource>(const Metadata&)>;
    void register_factory(ResourceType resourceType, FactoryFunc&& factoryFunc);

    void register_from_disk(const std::filesystem::path& filename);

    void add_resource(ResourceId id, std::unique_ptr<Resource>&& resource);

    auto get_metadata(ResourceId id) -> Metadata&;

    template <typename T>
    auto get_ptr(ResourceId id) -> ResourceHandle<T>;

    void load(ResourceId id);
    void unload(ResourceId id);

#pragma region Implementation

    template <typename T>
    auto get_ptr(ResourceId id) -> ResourceHandle<T>
    {
        auto& metadata = get_metadata(id);
        return ResourceHandle<T>(&metadata);
    }

#pragma endregion

}