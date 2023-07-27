#pragma once

#include "buffer.hpp"

namespace rune::rhi
{
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

    protected:
        virtual void on_cmd_list_reset(CommandList& cmdList) = 0;
        virtual void on_cmd_list_begin(CommandList& cmdList) = 0;
    };

}