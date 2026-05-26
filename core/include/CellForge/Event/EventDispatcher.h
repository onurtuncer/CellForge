// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

// core/event/EventDispatcher.h
#pragma once
#include "CellForge/Event/Event.h"

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