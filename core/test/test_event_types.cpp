// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <CellForge/Event/ApplicationEvent.h>
#include <CellForge/Event/KeyboardEvents.h>
#include <CellForge/Event/MouseEvents.h>
#include <CellForge/Event/EventQueue.h>

using namespace CellForge;

// ─── KeyPressedEvent ──────────────────────────────────────────────────────────

TEST_CASE("KeyPressedEvent: type and staticType agree", "[event][keyboard]") {
    KeyPressedEvent e(KeyCode::A);
    CHECK(e.type()       == EventType::KeyPressed);
    CHECK(KeyPressedEvent::staticType() == EventType::KeyPressed);
    CHECK(e.type()       == KeyPressedEvent::staticType());
}

TEST_CASE("KeyPressedEvent: stores keyCode", "[event][keyboard]") {
    KeyPressedEvent e(KeyCode::Space);
    CHECK(e.keyCode() == KeyCode::Space);
}

TEST_CASE("KeyPressedEvent: repeat defaults to false", "[event][keyboard]") {
    KeyPressedEvent e(KeyCode::A);
    CHECK(e.repeat() == false);
}

TEST_CASE("KeyPressedEvent: repeat flag is stored", "[event][keyboard]") {
    KeyPressedEvent repeated(KeyCode::A, true);
    CHECK(repeated.repeat() == true);
}

TEST_CASE("KeyPressedEvent: category is Input | Keyboard", "[event][keyboard]") {
    KeyPressedEvent e(KeyCode::A);
    CHECK(e.isInCategory(EventCategory::Input));
    CHECK(e.isInCategory(EventCategory::Keyboard));
    CHECK_FALSE(e.isInCategory(EventCategory::Mouse));
    CHECK_FALSE(e.isInCategory(EventCategory::Application));
}

TEST_CASE("KeyPressedEvent: handled defaults to false", "[event][keyboard]") {
    KeyPressedEvent e(KeyCode::A);
    CHECK(e.handled == false);
}

// ─── KeyReleasedEvent ─────────────────────────────────────────────────────────

TEST_CASE("KeyReleasedEvent: type and staticType agree", "[event][keyboard]") {
    KeyReleasedEvent e(KeyCode::B);
    CHECK(e.type()                      == EventType::KeyReleased);
    CHECK(KeyReleasedEvent::staticType() == EventType::KeyReleased);
}

TEST_CASE("KeyReleasedEvent: stores keyCode", "[event][keyboard]") {
    KeyReleasedEvent e(KeyCode::Enter);
    CHECK(e.keyCode() == KeyCode::Enter);
}

TEST_CASE("KeyReleasedEvent: category is Input | Keyboard", "[event][keyboard]") {
    KeyReleasedEvent e(KeyCode::A);
    CHECK(e.isInCategory(EventCategory::Input));
    CHECK(e.isInCategory(EventCategory::Keyboard));
    CHECK_FALSE(e.isInCategory(EventCategory::Mouse));
}

// ─── MouseButtonPressedEvent ──────────────────────────────────────────────────

TEST_CASE("MouseButtonPressedEvent: type and staticType agree", "[event][mouse]") {
    MouseButtonPressedEvent e(MouseButton::Left, 10.f, 20.f);
    CHECK(e.type()                           == EventType::MouseButtonPressed);
    CHECK(MouseButtonPressedEvent::staticType() == EventType::MouseButtonPressed);
}

TEST_CASE("MouseButtonPressedEvent: stores button and position", "[event][mouse]") {
    MouseButtonPressedEvent e(MouseButton::Right, 100.f, 200.f);
    CHECK(e.button() == MouseButton::Right);
    CHECK(e.x()      == Catch::Approx(100.f));
    CHECK(e.y()      == Catch::Approx(200.f));
}

TEST_CASE("MouseButtonPressedEvent: category is Input | MouseButton", "[event][mouse]") {
    MouseButtonPressedEvent e(MouseButton::Left, 0.f, 0.f);
    CHECK(e.isInCategory(EventCategory::Input));
    CHECK(e.isInCategory(EventCategory::MouseButton));
    CHECK_FALSE(e.isInCategory(EventCategory::Keyboard));
    CHECK_FALSE(e.isInCategory(EventCategory::Application));
}

// ─── MouseButtonReleasedEvent ─────────────────────────────────────────────────

TEST_CASE("MouseButtonReleasedEvent: type and staticType agree", "[event][mouse]") {
    MouseButtonReleasedEvent e(MouseButton::Left, 0.f, 0.f);
    CHECK(e.type()                            == EventType::MouseButtonReleased);
    CHECK(MouseButtonReleasedEvent::staticType() == EventType::MouseButtonReleased);
}

TEST_CASE("MouseButtonReleasedEvent: stores button and position", "[event][mouse]") {
    MouseButtonReleasedEvent e(MouseButton::Middle, 5.f, 15.f);
    CHECK(e.button() == MouseButton::Middle);
    CHECK(e.x()      == Catch::Approx(5.f));
    CHECK(e.y()      == Catch::Approx(15.f));
}

TEST_CASE("MouseButtonReleasedEvent: category is Input | MouseButton", "[event][mouse]") {
    MouseButtonReleasedEvent e(MouseButton::Left, 0.f, 0.f);
    CHECK(e.isInCategory(EventCategory::Input));
    CHECK(e.isInCategory(EventCategory::MouseButton));
    CHECK_FALSE(e.isInCategory(EventCategory::Keyboard));
}

// ─── MouseMovedEvent ──────────────────────────────────────────────────────────

TEST_CASE("MouseMovedEvent: type and staticType agree", "[event][mouse]") {
    MouseMovedEvent e(0.f, 0.f);
    CHECK(e.type()                  == EventType::MouseMoved);
    CHECK(MouseMovedEvent::staticType() == EventType::MouseMoved);
}

TEST_CASE("MouseMovedEvent: stores position", "[event][mouse]") {
    MouseMovedEvent e(320.f, 240.f);
    CHECK(e.x() == Catch::Approx(320.f));
    CHECK(e.y() == Catch::Approx(240.f));
}

TEST_CASE("MouseMovedEvent: category is Input | Mouse", "[event][mouse]") {
    MouseMovedEvent e(0.f, 0.f);
    CHECK(e.isInCategory(EventCategory::Input));
    CHECK(e.isInCategory(EventCategory::Mouse));
    CHECK_FALSE(e.isInCategory(EventCategory::MouseButton));
    CHECK_FALSE(e.isInCategory(EventCategory::Keyboard));
}

// ─── MouseScrolledEvent ───────────────────────────────────────────────────────

TEST_CASE("MouseScrolledEvent: type and staticType agree", "[event][mouse]") {
    MouseScrolledEvent e(0.f, 0.f);
    CHECK(e.type()                     == EventType::MouseScrolled);
    CHECK(MouseScrolledEvent::staticType() == EventType::MouseScrolled);
}

TEST_CASE("MouseScrolledEvent: stores offsets", "[event][mouse]") {
    MouseScrolledEvent e(1.5f, -2.0f);
    CHECK(e.xOffset() == Catch::Approx( 1.5f));
    CHECK(e.yOffset() == Catch::Approx(-2.0f));
}

TEST_CASE("MouseScrolledEvent: category is Input | Mouse", "[event][mouse]") {
    MouseScrolledEvent e(0.f, 0.f);
    CHECK(e.isInCategory(EventCategory::Input));
    CHECK(e.isInCategory(EventCategory::Mouse));
    CHECK_FALSE(e.isInCategory(EventCategory::MouseButton));
}

// ─── WindowResizeEvent ────────────────────────────────────────────────────────

TEST_CASE("WindowResizeEvent: type and staticType agree", "[event][window]") {
    WindowResizeEvent e(800, 600);
    CHECK(e.type()                      == EventType::WindowResize);
    CHECK(WindowResizeEvent::staticType() == EventType::WindowResize);
}

TEST_CASE("WindowResizeEvent: stores dimensions", "[event][window]") {
    WindowResizeEvent e(1920, 1080);
    CHECK(e.width()  == 1920u);
    CHECK(e.height() == 1080u);
}

TEST_CASE("WindowResizeEvent: category is Application", "[event][window]") {
    WindowResizeEvent e(800, 600);
    CHECK(e.isInCategory(EventCategory::Application));
    CHECK_FALSE(e.isInCategory(EventCategory::Input));
}

// ─── WindowMinimizeEvent ──────────────────────────────────────────────────────

TEST_CASE("WindowMinimizeEvent: type and staticType agree", "[event][window]") {
    WindowMinimizeEvent e(true);
    CHECK(e.type()                        == EventType::WindowMinimize);
    CHECK(WindowMinimizeEvent::staticType() == EventType::WindowMinimize);
}

TEST_CASE("WindowMinimizeEvent: stores minimized flag", "[event][window]") {
    CHECK(WindowMinimizeEvent(true).isMinimized()  == true);
    CHECK(WindowMinimizeEvent(false).isMinimized() == false);
}

// ─── WindowCloseEvent ─────────────────────────────────────────────────────────

TEST_CASE("WindowCloseEvent: type and staticType agree", "[event][window]") {
    WindowCloseEvent e;
    CHECK(e.type()                     == EventType::WindowClose);
    CHECK(WindowCloseEvent::staticType() == EventType::WindowClose);
}

TEST_CASE("WindowCloseEvent: category is Application", "[event][window]") {
    WindowCloseEvent e;
    CHECK(e.isInCategory(EventCategory::Application));
    CHECK_FALSE(e.isInCategory(EventCategory::Input));
}

// ─── AppUpdateEvent ───────────────────────────────────────────────────────────

TEST_CASE("AppUpdateEvent: type and staticType agree", "[event][app]") {
    AppUpdateEvent e;
    CHECK(e.type()                    == EventType::AppUpdate);
    CHECK(AppUpdateEvent::staticType() == EventType::AppUpdate);
}

TEST_CASE("AppUpdateEvent: category is Application", "[event][app]") {
    AppUpdateEvent e;
    CHECK(e.isInCategory(EventCategory::Application));
    CHECK_FALSE(e.isInCategory(EventCategory::Input));
}

// ─── isInCategory: None category is never matched ─────────────────────────────

TEST_CASE("Event: isInCategory returns false for EventCategory::None", "[event]") {
    KeyPressedEvent e(KeyCode::A);
    CHECK_FALSE(e.isInCategory(EventCategory::None));
}

// ─── EventQueue ───────────────────────────────────────────────────────────────

TEST_CASE("EventQueue: dispatch is a no-op before setCallback", "[event][queue]") {
    EventQueue queue;
    KeyPressedEvent e(KeyCode::A);
    REQUIRE_NOTHROW(queue.dispatch(e));
    CHECK(e.handled == false);
}

TEST_CASE("EventQueue: dispatch calls the registered callback", "[event][queue]") {
    EventQueue queue;
    bool called = false;
    queue.setCallback([&called](Event&) { called = true; });
    KeyPressedEvent e(KeyCode::A);
    queue.dispatch(e);
    CHECK(called == true);
}

TEST_CASE("EventQueue: callback receives the correct event reference", "[event][queue]") {
    EventQueue queue;
    EventType received = EventType::None;
    queue.setCallback([&received](Event& ev) { received = ev.type(); });
    MouseScrolledEvent e(1.f, 2.f);
    queue.dispatch(e);
    CHECK(received == EventType::MouseScrolled);
}

TEST_CASE("EventQueue: replacing callback discards the old one", "[event][queue]") {
    EventQueue queue;
    int first = 0, second = 0;
    queue.setCallback([&first](Event&)  { ++first; });
    queue.setCallback([&second](Event&) { ++second; });
    WindowCloseEvent e;
    queue.dispatch(e);
    CHECK(first  == 0);
    CHECK(second == 1);
}

TEST_CASE("EventQueue: dispatches multiple events in sequence", "[event][queue]") {
    EventQueue queue;
    int count = 0;
    queue.setCallback([&count](Event&) { ++count; });
    WindowCloseEvent a;
    AppUpdateEvent   b;
    KeyPressedEvent  c(KeyCode::Space);
    queue.dispatch(a);
    queue.dispatch(b);
    queue.dispatch(c);
    CHECK(count == 3);
}
