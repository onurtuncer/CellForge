// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#include "ViewerApplication.h"
#include "IViewerWidget.h"

#include <CellForge/Log.h>
#include <CellForge/Event/Event.h>
#include <CellForge/Event/EventDispatcher.h>
#include <CellForge/Event/MouseEvents.h>
#include <CellForge/Event/ApplicationEvent.h>
#include <CellForge/IWindow.h>

#include <format>
#include <functional>

namespace CellForge {

void ViewerApplication::wireViewer(IWindow* window, IViewerWidget* viewer)
{
    m_window = window;
    m_viewer = viewer;

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
}

void ViewerApplication::OnUpdate()
{
    if (m_window && !m_window->IsOpen())
        Close();
}

void ViewerApplication::OnShutdown()
{
    CF_CORE_INFO("Application shutting down.");
}

} // namespace CellForge
