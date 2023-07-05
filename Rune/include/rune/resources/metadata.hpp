#pragma once

#include "common.hpp"
#include "types.hpp"
#include "resource.hpp"

#include <atomic>
#include <filesystem>

namespace rune
{
    enum class ResourceState
    {
        NotLoaded,
        Loading,
        Loaded,
    };

    enum class ResourceSource
    {
        eNone,
        eDisk,
        eApp
    };

    struct ResourceMetadata
    {
        ResourceType type{};
        std::atomic<ResourceState> state{ ResourceState::NotLoaded };
        ResourceSource source{ ResourceSource::eNone };
        std::filesystem::path sourceFilename{};
        sizet sourceOffset;
        sizet sourceSize;
        std::unique_ptr<Resource> resource{ nullptr };
        std::vector<ResourceId> dependencies{};
    };
}