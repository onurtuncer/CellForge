// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

// Demonstrates CellForge Application owning the event loop via QtApplicationPlatform,
// with an OCCT 3D viewer hosted inside a QMainWindow.
//   - Application::Run() drives the loop; QApplication::exec() is never called.
//   - ViewerWidget (platform/qt) translates Qt/OCCT events into typed CellForge events.
//   - Events are routed through Application::OnEvent() and logged to the console.

#include <CellForge/Application.h>
#include <CellForge/EntryPoint.h>
#include <CellForge/Log.h>
#include <CellForge/Event/EventDispatcher.h>
#include <CellForge/Event/MouseEvents.h>
#include <CellForge/Event/ApplicationEvent.h>
#include <CellForge/qt/QtApplicationPlatform.h>
#include <CellForge/qt/widgets/ViewerWidget.h>

#include <QMainWindow>
#include <QWidget>
#include <QHBoxLayout>

namespace CellForge {

class ViewerApp : public Application {
public:
    ViewerApp(int argc, char** argv)
        : Application({"CellForge Viewer"},
                      std::make_unique<QtApplicationPlatform>(argc, argv))
    {}

    void OnInit() override
    {
        m_window = new QMainWindow();
        m_window->setWindowTitle("CellForge — Qt Viewer Example");

        QWidget* central = new QWidget(m_window);
        m_window->setCentralWidget(central);

        auto* layout = new QHBoxLayout(central);
        layout->setContentsMargins(0, 0, 0, 0);

        m_viewer = new ViewerWidget(central);
        layout->addWidget(m_viewer);

        m_viewer->setEventCallback([this](Event& e) { OnEvent(e); });

        AddEventCallback([](Event& event) {
            EventDispatcher d(event);

            d.dispatch<MouseButtonReleasedEvent>([](MouseButtonReleasedEvent& e) {
                CF_CORE_INFO("[MouseReleased] button={} x={:.1f} y={:.1f}",
                    static_cast<int>(e.button()), e.x(), e.y());
                return false;
            });
            d.dispatch<MouseButtonPressedEvent>([](MouseButtonPressedEvent& e) {
                CF_CORE_INFO("[MousePressed]  button={} x={:.1f} y={:.1f}",
                    static_cast<int>(e.button()), e.x(), e.y());
                return false;
            });
            d.dispatch<WindowResizeEvent>([](WindowResizeEvent& e) {
                CF_CORE_INFO("[WindowResize]  {}x{}", e.width(), e.height());
                return false;
            });
        });

        m_window->resize(900, 600);
        m_window->show();

        CF_CORE_INFO("Viewer application running — click inside the viewport.");
    }

    void OnUpdate() override
    {
        if (m_window && !m_window->isVisible())
            Close();
    }

    void OnShutdown() override
    {
        CF_CORE_INFO("Application shutting down.");
        delete m_window;
        m_window = nullptr;
    }

private:
    QMainWindow*  m_window = nullptr;
    ViewerWidget* m_viewer = nullptr;
};

Application* CreateApplication(int argc, char** argv)
{
    return new ViewerApp(argc, argv);
}

} // namespace CellForge
