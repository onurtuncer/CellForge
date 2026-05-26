// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

// Demonstrates CellForge Application owning the event loop via QtApplicationPlatform:
//   - Application::Run() drives the loop; QApplication::exec() is never called.
//   - ViewportWidget translates Qt input/window events into typed CellForge events.
//   - Events are routed through Application::OnEvent() and logged to the console.

#include <CellForge/Application.h>
#include <CellForge/EntryPoint.h>
#include <CellForge/Log.h>
#include <CellForge/Event/EventDispatcher.h>
#include <CellForge/Event/KeyboardEvents.h>
#include <CellForge/Event/MouseEvents.h>
#include <CellForge/Event/ApplicationEvent.h>
#include <CellForge/qt/QtApplicationPlatform.h>
#include <CellForge/qt/widgets/ViewPortWidget.h>

#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QStatusBar>

namespace CellForge {

class EventLogApp : public Application {
public:
    EventLogApp(int argc, char** argv)
        : Application({"Event Log Demo"},
                      std::make_unique<QtApplicationPlatform>(argc, argv))
    {}

    void OnInit() override
    {
        m_Window = new QMainWindow();
        m_Window->setWindowTitle("CellForge \xe2\x80\x94 Event Log Demo");
        m_Window->resize(800, 600);
        m_Window->statusBar()->showMessage(
            "Click, type, scroll, or resize the viewport \xe2\x80\x94 events are logged to the console.");

        QWidget* central = new QWidget(m_Window);
        auto*    layout  = new QVBoxLayout(central);

        auto* hint = new QLabel(
            "<b>Viewport</b> \xe2\x80\x94 click or type here. "
            "Events are translated to CellForge events and logged to stdout.", central);
        hint->setWordWrap(true);
        layout->addWidget(hint);

        m_Viewport = new ViewportWidget(central);
        m_Viewport->setMinimumSize(600, 400);
        m_Viewport->setStyleSheet("background-color: #1e1e2e; border: 1px solid #45475a;");
        m_Viewport->setEventCallback([this](Event& e) { OnEvent(e); });
        layout->addWidget(m_Viewport, 1);

        m_Window->setCentralWidget(central);
        m_Window->show();
        m_Viewport->setFocus();

        // Log each event type at an appropriate level.
        AddEventCallback([](Event& event) {
            EventDispatcher d(event);

            d.dispatch<KeyPressedEvent>([](KeyPressedEvent& e) {
                CF_CORE_INFO("[KeyPressed]    keyCode={} repeat={}",
                    static_cast<int>(e.keyCode()), e.repeat());
                return false;
            });
            d.dispatch<KeyReleasedEvent>([](KeyReleasedEvent& e) {
                CF_CORE_INFO("[KeyReleased]   keyCode={}",
                    static_cast<int>(e.keyCode()));
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
                CF_CORE_TRACE("[MouseMoved]    x={:.1f} y={:.1f}", e.x(), e.y());
                return false;
            });
            d.dispatch<MouseScrolledEvent>([](MouseScrolledEvent& e) {
                CF_CORE_INFO("[MouseScrolled] dx={:.2f} dy={:.2f}",
                    e.xOffset(), e.yOffset());
                return false;
            });
            d.dispatch<WindowResizeEvent>([](WindowResizeEvent& e) {
                CF_CORE_INFO("[WindowResize]  {}x{}", e.width(), e.height());
                return false;
            });
        });

        CF_CORE_INFO("Application running \xe2\x80\x94 interact with the viewport.");
    }

    void OnUpdate() override
    {
        // Detect main-window close: Qt hides it on the close button click.
        // QApplication::exec() would normally handle quit; we do it here instead.
        if (m_Window && !m_Window->isVisible())
            Close();
    }

    void OnShutdown() override
    {
        CF_CORE_INFO("Application shutting down.");
        delete m_Window;
        m_Window   = nullptr;
        m_Viewport = nullptr; // owned by m_Window's widget tree, already deleted
    }

private:
    QMainWindow*    m_Window   = nullptr;
    ViewportWidget* m_Viewport = nullptr;
};

Application* CreateApplication(int argc, char** argv)
{
    return new EventLogApp(argc, argv);
}

} // namespace CellForge
