// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

// core/event/Event.h
#pragma once

#include "CellForge/Event/EventType.h"

namespace CellForge {

class Event {
public:
    virtual ~Event() = default;

    virtual EventType type() const = 0;
    virtual uint32_t categoryFlags() const = 0;

    bool handled = false;

    bool isInCategory(EventCategory category) const {
        return categoryFlags() & static_cast<uint32_t>(category);
    }
};

}