#pragma once

#include "buffer.hpp"

#ifdef RUNE_GRAPHICS_VULKAN
    #include <vulkan/vulkan.hpp>
#else
    #error NO GRAPHICS API IN USE.
#endif

namespace rune::rhi
{
    class SurfaceDecl;
    class Surface;
    class CommandList;
    class Fence;

    /*
     *  RHI Core - Device represents ????.
     *   - Subclasses should be thread-safe.
     */
    class Device
    {
    public:
        static auto create() -> Owned<Device>;

        virtual auto create_surface(const SurfaceDecl& decl) -> Owned<Surface> = 0;
        virtual auto create_cmd_list(bool autoSubmit = true) -> Owned<CommandList> = 0;

        /**
         * @brief Submits all command lists that have begun. They are submitted in the order that each CommandList::begin() was called.
         * @param
         * @return void
         */
        virtual void submit(Fence* fence = nullptr, u64 fenceValue = 0) = 0;
        /**
         * @brief Submit a single command list.
         * @param cmdList
         */
        virtual void submit_single(CommandList& cmdList, Fence* fence, u64 fenceValue) = 0;

        // void mark_for_destruction(const Shared<Resource>& resource);

    protected:
        virtual void on_cmd_list_reset(CommandList& cmdList) = 0;
        virtual void on_cmd_list_begin(CommandList& cmdList) = 0;
    };

}