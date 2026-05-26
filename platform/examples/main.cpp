// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

// Demonstrates the CellForge event abstraction layer over Qt:
//   - ViewportWidget catches Qt input/window events
//   - Each event is translated to a typed CellForge event
//   - Application::OnEvent logs it to the core logger
//
// The Qt event loop drives everything; CellForge::Application::Run() is not
// called here because merging two blocking loops is a separate architectural step.

#include <CellForge/Application.h>
#include <CellForge/Log.h>
#include <CellForge/Event/EventDispatcher.h>
#include <CellForge/Event/KeyboardEvents.h>
#include <CellForge/Event/MouseEvents.h>
#include <CellForge/Event/ApplicationEvent.h>
#include <CellForge/qt/widgets/ViewPortWidget.h>

#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QStatusBar>

// ─── Logging helper ───────────────────────────────────────────────────────────

static void logEvent(CellForge::Event& event)
{
    using namespace CellForge;
    EventDispatcher d(event);

    d.dispatch<KeyPressedEvent>([](KeyPressedEvent& e) {
        CF_CORE_INFO("[KeyPressed]  keyCode={} repeat={}", static_cast<int>(e.keyCode()), e.repeat());
        return false;
    });
    d.dispatch<KeyReleasedEvent>([](KeyReleasedEvent& e) {
        CF_CORE_INFO("[KeyReleased] keyCode={}", static_cast<int>(e.keyCode()));
        return false;
    });
    d.dispatch<MouseButtonPressedEvent>([](MouseButtonPressedEvent& e) {
        CF_CORE_INFO("[MousePressed]  button={} x={:.1f} y={:.1f}",
            static_cast<int>(e.button()), e.x(), e.y());
        return false;
    });
    d.dispatch<MouseButtonReleasedEvent>([](MouseButtonReleasedEvent& e) {
        CF_CORE_INFO("[MouseReleased] button={} x={:.1f} y={:.1f}",
            static_cast<int>(e.button()), e.x(), e.y());
        return false;
    });
    d.dispatch<MouseMovedEvent>([](MouseMovedEvent& e) {
        CF_CORE_TRACE("[MouseMoved]   x={:.1f} y={:.1f}", e.x(), e.y());
        return false;
    });
    d.dispatch<MouseScrolledEvent>([](MouseScrolledEvent& e) {
        CF_CORE_INFO("[MouseScrolled] dx={:.2f} dy={:.2f}", e.xOffset(), e.yOffset());
        return false;
    });
    d.dispatch<WindowResizeEvent>([](WindowResizeEvent& e) {
        CF_CORE_INFO("[WindowResize]  {}x{}", e.width(), e.height());
        return false;
    });
    d.dispatch<WindowCloseEvent>([](WindowCloseEvent&) {
        CF_CORE_INFO("[WindowClose]");
        return false;
    });
}

// ─── Entry point ──────────────────────────────────────────────────────────────

int main(int argc, char** argv)
{
    // Init CellForge logging before anything else.
    CellForge::Log::Init();
    CF_CORE_INFO("CellForge platform_qt event demo starting.");

    QApplication qtApp(argc, argv);

    // Main window
    QMainWindow window;
    window.setWindowTitle("CellForge — Event Log Demo");
    window.resize(800, 600);

    // Status bar hint
    window.statusBar()->showMessage(
        "Click, type, scroll, or resize the viewport — events are logged to the console.");

    // Central area: label above + viewport below
    QWidget* central = new QWidget(&window);
    QVBoxLayout* layout = new QVBoxLayout(central);

    QLabel* hint = new QLabel(
        "<b>Viewport</b> — click or type here. "
        "Events are translated to CellForge events and logged to stdout.", central);
    hint->setWordWrap(true);
    layout->addWidget(hint);

    // The CellForge viewport widget
    CellForge::ViewportWidget* viewport = new CellForge::ViewportWidget(central);
    viewport->setMinimumSize(600, 400);
    viewport->setStyleSheet("background-color: #1e1e2e; border: 1px solid #45475a;");
    viewport->setEventCallback(logEvent);
    layout->addWidget(viewport, 1);

    window.setCentralWidget(central);
    window.show();

    // Give the viewport keyboard focus immediately.
    viewport->setFocus();

    CF_CORE_INFO("Qt event loop starting — interact with the viewport.");
    int result = qtApp.exec();

    CF_CORE_INFO("Qt event loop exited (code {}).", result);
    CellForge::Log::Shutdown();
    return result;
}
