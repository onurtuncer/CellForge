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
#include "CellForge/Input/KeyCodes.h"

namespace CellForge {

class KeyEvent : public Event {
public:
    KeyCode keyCode() const { return m_KeyCode; }

    uint32_t categoryFlags() const override {
        return static_cast<uint32_t>(EventCategory::Input) |
               static_cast<uint32_t>(EventCategory::Keyboard);
    }

protected:
    explicit KeyEvent(KeyCode keyCode)
        : m_KeyCode(keyCode) {}

private:
    KeyCode m_KeyCode;
};

class KeyPressedEvent : public KeyEvent {
public:
    KeyPressedEvent(KeyCode keyCode, bool repeat = false)
        : KeyEvent(keyCode), m_Repeat(repeat) {}

    static EventType staticType() { return EventType::KeyPressed; }
    EventType type() const override { return EventType::KeyPressed; }

    bool repeat() const { return m_Repeat; }

private:
    bool m_Repeat;
};

class KeyReleasedEvent : public KeyEvent {
public:
    explicit KeyReleasedEvent(KeyCode keyCode)
        : KeyEvent(keyCode) {}

    static EventType staticType() { return EventType::KeyReleased; }
    EventType type() const override { return EventType::KeyReleased; }
};

}