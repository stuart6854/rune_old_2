#include "rune/events/events.hpp"

namespace rune
{
    void Events::update()
    {
        while (!m_eventQueue.empty())
        {
            const auto& event = m_eventQueue.front();
            for (auto& subscriber : m_subscribers)
            {
                bool handled = subscriber(event);
                if (handled)
                    break;
            }
            m_eventQueue.pop();
        }
    }

    void Events::subscribe(EventCallbackFn&& callback)
    {
        m_subscribers.push_back(callback);
    }

    void Events::post(const Event& event)
    {
        m_eventQueue.push(event);
    }

}