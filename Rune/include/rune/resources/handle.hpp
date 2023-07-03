#pragma once

#include "common.hpp"
#include "metadata.hpp"

namespace rune::resources
{
    template <typename T>
    class ResourceHandle
    {
    public:
        ResourceHandle() = default;
        explicit ResourceHandle(Metadata* metadata);
        ~ResourceHandle() = default;

        auto get() const -> T*;

        /* Does this handle point to a resources metadata? */
        bool is_valid() const;
        /* Is the resource this handle points to loaded? */
        bool is_loaded() const;

        /* Operators */

        auto operator*() const -> T&;
        auto operator->() -> T*;

    private:
        Metadata* m_metadata{ nullptr };
    };

    template <typename T>
    ResourceHandle<T>::ResourceHandle(Metadata* metadata) : m_metadata(metadata)
    {
    }

    template <typename T>
    auto ResourceHandle<T>::get() const -> T*
    {
        return static_cast<T*>(m_metadata->resource.get());
    }

    template <typename T>
    bool ResourceHandle<T>::is_valid() const
    {
        return m_metadata != nullptr;
    }

    template <typename T>
    bool ResourceHandle<T>::is_loaded() const
    {
        return is_valid() && m_metadata->state == ResourceState::Loaded;
    }

    template <typename T>
    auto ResourceHandle<T>::operator*() const -> T&
    {
        return *get();
    }

    template <typename T>
    auto ResourceHandle<T>::operator->() -> T*
    {
        return get();
    }

}