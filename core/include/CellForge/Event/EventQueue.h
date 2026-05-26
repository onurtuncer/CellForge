// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#include "CellForge/Event/Event.h"

#include <functional>

namespace CellForge {

// Thin callback holder used by platform widgets to forward translated events.
// The platform widget owns one EventQueue; the application sets the callback
// once via setCallback(), then every dispatch() call routes straight to OnEvent.
// NOTE: "emit" is intentionally avoided — Qt defines it as a macro (expands to empty).
class EventQueue {
public:
    using CallbackFn = std::function<void(Event&)>;

    void setCallback(CallbackFn cb) { m_Callback = std::move(cb); }

    void dispatch(Event& e) const {
        if (m_Callback)
            m_Callback(e);
    }

private:
    CallbackFn m_Callback;
};

} // namespace CellForge
