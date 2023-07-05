#include "events/system_events.hpp"

#include "common_internal.hpp"

namespace rune
{
    void SystemEvents::initialise()
    {
        LOG_INFO("Events system initialised.");
    }

    void SystemEvents::update()
    {
        while (!m_messages.empty())
        {
            const auto& msg = m_messages.front();
            for (const auto& receieverFunc : m_receivers)
            {
                receieverFunc(msg);
            }
            m_messages.pop();
        }
    }

    void SystemEvents::shutdown()
    {
        LOG_INFO("Events system has shut down.");
    }

    void SystemEvents::register_receiver(SystemEvents::ReceiveEventFunc&& receiveEventFunc)
    {
        m_receivers.push_back(std::move(receiveEventFunc));
    }

    void SystemEvents::post_event(const Event& msg)
    {
        m_messages.push(msg);
    }

}