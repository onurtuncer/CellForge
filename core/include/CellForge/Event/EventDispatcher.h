

// core/event/EventDispatcher.h
#pragma once
#include "core/event/Event.h"

namespace CellForge {

class EventDispatcher {
public:
    explicit EventDispatcher(Event& event)
        : m_Event(event) {}

    template <typename T, typename Func>
    bool dispatch(Func&& func) {
        if (m_Event.type() == T::staticType()) {
            m_Event.handled = func(static_cast<T&>(m_Event));
            return true;
        }

        return false;
    }

private:
    Event& m_Event;
};

}