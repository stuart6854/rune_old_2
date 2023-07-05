#pragma once

#include "common.hpp"
#include "core/system.hpp"

#include <vector>
#include <memory>
#include <filesystem>
#include <string_view>

namespace rune
{
    enum class LoadMethod
    {
        Normal,
        Additive,
    };

    using Entity = u32;

    class SystemScene : public ISystem
    {
    public:
        SystemScene();
        ~SystemScene() override;

        void initialise() override;
        void update() override;
        void shutdown() override;

        void clear_scene();

        void load_scene(std::string_view filename, LoadMethod loadMethod = LoadMethod::Normal);
        void unload_scene();

        auto create_entity() -> Entity;
        void destroy_entity(Entity entity);

    private:
        void load_yaml_scene(std::filesystem::path filename);

    private:
        struct Pimpl;
        std::unique_ptr<Pimpl> m_pimpl;
    };
}