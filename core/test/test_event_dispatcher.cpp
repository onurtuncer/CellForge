// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#include <catch2/catch_test_macros.hpp>

#include <CellForge/Event/EventDispatcher.h>
#include <CellForge/Event/ApplicationEvent.h>
#include <CellForge/Event/KeyboardEvents.h>

using namespace CellForge;

// ─── Minimal concrete events for dispatch tests ───────────────────────────────

struct PingEvent : Event {
    explicit PingEvent(int v = 0) : value(v) {}
    int value;
    static EventType staticType()      { return EventType::AppUpdate; }
    EventType type() const override    { return EventType::AppUpdate; }
    uint32_t categoryFlags() const override {
        return static_cast<uint32_t>(EventCategory::Application);
    }
};

struct PongEvent : Event {
    static EventType staticType()      { return EventType::AppTick; }
    EventType type() const override    { return EventType::AppTick; }
    uint32_t categoryFlags() const override {
        return static_cast<uint32_t>(EventCategory::Application);
    }
};

// ─── Routing ──────────────────────────────────────────────────────────────────

TEST_CASE("EventDispatcher: dispatch returns true on type match", "[event][dispatcher]") {
    PingEvent e;
    EventDispatcher d(e);
    bool result = d.dispatch<PingEvent>([](PingEvent&) { return false; });
    CHECK(result == true);
}

TEST_CASE("EventDispatcher: dispatch returns false on type mismatch", "[event][dispatcher]") {
    PingEvent e;
    EventDispatcher d(e);
    bool result = d.dispatch<PongEvent>([](PongEvent&) { return false; });
    CHECK(result == false);
}

TEST_CASE("EventDispatcher: handler is called on type match", "[event][dispatcher]") {
    PingEvent e;
    EventDispatcher d(e);
    bool called = false;
    d.dispatch<PingEvent>([&called](PingEvent&) { called = true; return false; });
    CHECK(called == true);
}

TEST_CASE("EventDispatcher: handler is NOT called on type mismatch", "[event][dispatcher]") {
    PingEvent e;
    EventDispatcher d(e);
    bool called = false;
    d.dispatch<PongEvent>([&called](PongEvent&) { called = true; return false; });
    CHECK(called == false);
}

// ─── handled flag ─────────────────────────────────────────────────────────────

TEST_CASE("EventDispatcher: handled is false by default", "[event][dispatcher]") {
    PingEvent e;
    CHECK(e.handled == false);
}

TEST_CASE("EventDispatcher: handled is set to true when handler returns true", "[event][dispatcher]") {
    PingEvent e;
    EventDispatcher d(e);
    d.dispatch<PingEvent>([](PingEvent&) { return true; });
    CHECK(e.handled == true);
}

TEST_CASE("EventDispatcher: handled is set to false when handler returns false", "[event][dispatcher]") {
    PingEvent e;
    e.handled = true;
    EventDispatcher d(e);
    d.dispatch<PingEvent>([](PingEvent&) { return false; });
    CHECK(e.handled == false);
}

TEST_CASE("EventDispatcher: handled is unchanged on type mismatch", "[event][dispatcher]") {
    PingEvent e;
    e.handled = true;
    EventDispatcher d(e);
    d.dispatch<PongEvent>([](PongEvent&) { return false; });
    CHECK(e.handled == true);
}

// ─── Handler receives correctly typed event ───────────────────────────────────

TEST_CASE("EventDispatcher: handler receives the correct event data", "[event][dispatcher]") {
    PingEvent e(42);
    EventDispatcher d(e);
    int received = 0;
    d.dispatch<PingEvent>([&received](PingEvent& p) {
        received = p.value;
        return false;
    });
    CHECK(received == 42);
}

// ─── Multiple dispatch calls on the same event ────────────────────────────────

TEST_CASE("EventDispatcher: only the matching dispatch fires among multiple calls", "[event][dispatcher]") {
    PingEvent e;
    EventDispatcher d(e);

    bool pingCalled = false;
    bool pongCalled = false;

    d.dispatch<PingEvent>([&pingCalled](PingEvent&) { pingCalled = true; return false; });
    d.dispatch<PongEvent>([&pongCalled](PongEvent&) { pongCalled = true; return false; });

    CHECK(pingCalled  == true);
    CHECK(pongCalled == false);
}

TEST_CASE("EventDispatcher: two dispatch<T> calls for the same type both fire", "[event][dispatcher]") {
    PingEvent e;
    EventDispatcher d(e);

    int count = 0;
    d.dispatch<PingEvent>([&count](PingEvent&) { ++count; return false; });
    d.dispatch<PingEvent>([&count](PingEvent&) { ++count; return false; });
    CHECK(count == 2);
}

// ─── Works with real event types ──────────────────────────────────────────────

TEST_CASE("EventDispatcher: dispatches KeyPressedEvent correctly", "[event][dispatcher]") {
    KeyPressedEvent e(KeyCode::A, false);
    EventDispatcher d(e);

    KeyCode received = KeyCode::None;
    d.dispatch<KeyPressedEvent>([&received](KeyPressedEvent& k) {
        received = k.keyCode();
        return true;
    });

    CHECK(received == KeyCode::A);
    CHECK(e.handled == true);
}

TEST_CASE("EventDispatcher: WindowResizeEvent is not matched by KeyPressedEvent dispatch", "[event][dispatcher]") {
    WindowResizeEvent e(1920, 1080);
    EventDispatcher d(e);
    bool called = false;
    d.dispatch<KeyPressedEvent>([&called](KeyPressedEvent&) { called = true; return false; });
    CHECK(called == false);
}
