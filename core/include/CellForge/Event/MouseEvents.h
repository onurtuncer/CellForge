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

class MouseButtonPressedEvent : public Event {
public:
    MouseButtonPressedEvent(MouseButton button, float x, float y)
        : m_Button(button), m_X(x), m_Y(y) {}

    MouseButton button() const { return m_Button; }
    float x() const { return m_X; }
    float y() const { return m_Y; }

    static EventType staticType() { return EventType::MouseButtonPressed; }
    EventType type() const override { return EventType::MouseButtonPressed; }
    uint32_t categoryFlags() const override {
        return static_cast<uint32_t>(EventCategory::Input) |
               static_cast<uint32_t>(EventCategory::MouseButton);
    }

private:
    MouseButton m_Button;
    float m_X, m_Y;
};

class MouseButtonReleasedEvent : public Event {
public:
    MouseButtonReleasedEvent(MouseButton button, float x, float y)
        : m_Button(button), m_X(x), m_Y(y) {}

    MouseButton button() const { return m_Button; }
    float x() const { return m_X; }
    float y() const { return m_Y; }

    static EventType staticType() { return EventType::MouseButtonReleased; }
    EventType type() const override { return EventType::MouseButtonReleased; }
    uint32_t categoryFlags() const override {
        return static_cast<uint32_t>(EventCategory::Input) |
               static_cast<uint32_t>(EventCategory::MouseButton);
    }

private:
    MouseButton m_Button;
    float m_X, m_Y;
};

class MouseMovedEvent : public Event {
public:
    MouseMovedEvent(float x, float y)
        : m_X(x), m_Y(y) {}

    float x() const { return m_X; }
    float y() const { return m_Y; }

    static EventType staticType() { return EventType::MouseMoved; }
    EventType type() const override { return EventType::MouseMoved; }
    uint32_t categoryFlags() const override {
        return static_cast<uint32_t>(EventCategory::Input) |
               static_cast<uint32_t>(EventCategory::Mouse);
    }

private:
    float m_X, m_Y;
};

class MouseScrolledEvent : public Event {
public:
    MouseScrolledEvent(float xOffset, float yOffset)
        : m_XOffset(xOffset), m_YOffset(yOffset) {}

    float xOffset() const { return m_XOffset; }
    float yOffset() const { return m_YOffset; }

    static EventType staticType() { return EventType::MouseScrolled; }
    EventType type() const override { return EventType::MouseScrolled; }
    uint32_t categoryFlags() const override {
        return static_cast<uint32_t>(EventCategory::Input) |
               static_cast<uint32_t>(EventCategory::Mouse);
    }

private:
    float m_XOffset, m_YOffset;
};

} // namespace CellForge
