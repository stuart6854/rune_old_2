#pragma once

#include "common.hpp"
#include "handle.hpp"

#include <memory>

namespace rune::resources
{
    void initialise();
    void shutdown();

    void register_from_disk(const std::filesystem::path& filename);

    void add_resource(ResourceId id, std::unique_ptr<Resource>&& resource);

    auto get_metadata(ResourceId id) -> Metadata&;

    template <typename T>
    auto get_ptr(ResourceId id) -> ResourceHandle<T>;

#pragma region Implementation

    template <typename T>
    auto get_ptr(ResourceId id) -> ResourceHandle<T>
    {
        auto& metadata = get_metadata(id);
        return ResourceHandle<T>(&metadata);
    }

#pragma endregion

}