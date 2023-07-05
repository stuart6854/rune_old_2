#pragma once

#include "common.hpp"
#include "system.hpp"
#include "platform/system_platform.hpp"
#include "entry_point.hpp"

#include <memory>
#include <algorithm>

namespace rune
{
    class Engine
    {
    public:
        static auto get() -> Engine&;

        Engine(const EngineConfig& config);
        ~Engine();

        void run();

        void request_shutdown();

        auto get_primary_window() const -> WindowHandle { return m_primaryWindow; }
        auto get_delta_time() const -> f32 { return m_deltaTime; }

        template <SystemType T>
        void provide_system(std::unique_ptr<T>&& system);

        template <SystemType T>
        auto get_system() -> T*;

        template <SystemType T>
        void set_system_priority(u32 newPriority);

    private:
        void initialise();
        void update();
        void shutdown();

    private:
        EngineConfig m_config{};
        bool m_isRunning{ false };

        f32 m_deltaTime{};
        WindowHandle m_primaryWindow{};

        std::unordered_map<sizet, std::unique_ptr<ISystem>> m_systemMap{};
        std::vector<std::pair<sizet, u32>> m_systemPriorityVec{};
    };

    template <SystemType T>
    void Engine::provide_system(std::unique_ptr<T>&& system)
    {
        // #TODO: Shutdown pre-existing system??
        m_systemMap[typeid(T).hash_code()] = std::move(system);
    }

    template <SystemType T>
    auto Engine::get_system() -> T*
    {
        auto it = m_systemMap.find(typeid(T).hash_code());
        if (it == m_systemMap.end())
        {
            return nullptr;
        }
        return static_cast<T*>(it->second.get());
    }

    template <SystemType T>
    void Engine::set_system_priority(u32 newPriority)
    {
        const auto systemHash = typeid(T).hash_code();
        bool found = false;
        for (auto& [system, priority] : m_systemPriorityVec)
        {
            if (system == systemHash)
            {
                priority = newPriority;
                found = true;
            }
        }
        if (!found)
        {
            m_systemPriorityVec.emplace_back(systemHash, newPriority);
        }

        std::ranges::sort(m_systemPriorityVec,
                          [](const std::pair<sizet, u32>& a, const std::pair<sizet, u32>& b) { return a.second > b.second; });
    }

}
