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

#include <cstdint>

namespace CellForge {

class WindowResizeEvent : public Event {
public:
    WindowResizeEvent(uint32_t width, uint32_t height)
        : m_Width(width), m_Height(height) {}

    uint32_t width()  const { return m_Width; }
    uint32_t height() const { return m_Height; }

    static EventType staticType() { return EventType::WindowResize; }
    EventType type() const override { return EventType::WindowResize; }
    uint32_t categoryFlags() const override {
        return static_cast<uint32_t>(EventCategory::Application);
    }

private:
    uint32_t m_Width, m_Height;
};

class WindowMinimizeEvent : public Event {
public:
    explicit WindowMinimizeEvent(bool minimized) : m_Minimized(minimized) {}

    bool isMinimized() const { return m_Minimized; }

    static EventType staticType() { return EventType::WindowMinimize; }
    EventType type() const override { return EventType::WindowMinimize; }
    uint32_t categoryFlags() const override {
        return static_cast<uint32_t>(EventCategory::Application);
    }

private:
    bool m_Minimized = false;
};

class WindowCloseEvent : public Event {
public:
    WindowCloseEvent() = default;

    static EventType staticType() { return EventType::WindowClose; }
    EventType type() const override { return EventType::WindowClose; }
    uint32_t categoryFlags() const override {
        return static_cast<uint32_t>(EventCategory::Application);
    }
};

class AppUpdateEvent : public Event {
public:
    AppUpdateEvent() = default;

    static EventType staticType() { return EventType::AppUpdate; }
    EventType type() const override { return EventType::AppUpdate; }
    uint32_t categoryFlags() const override {
        return static_cast<uint32_t>(EventCategory::Application);
    }
};

} // namespace CellForge
