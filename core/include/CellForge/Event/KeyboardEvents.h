

#pragma once
#include "core/event/Event.h"
#include "core/event/input/KeyCode.h"

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

    EventType type() const override {
        return EventType::KeyPressed;
    }

    bool repeat() const {
        return m_Repeat;
    }

private:
    bool m_Repeat;
};

class KeyReleasedEvent : public KeyEvent {
public:
    explicit KeyReleasedEvent(KeyCode keyCode)
        : KeyEvent(keyCode) {}

    EventType type() const override {
        return EventType::KeyReleased;
    }
};

}