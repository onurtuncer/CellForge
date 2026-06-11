// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#include <CellForge/Application.h>
#include <CellForge/IWindow.h>
#include <CellForge/IViewerWidget.h>

namespace CellForge {

// Base class for Applications that own an OCCT viewer window.
//
// Provides:
//   - wireViewer()  — wires the viewer's event callback into OnEvent() and
//                     registers the standard mouse/resize logging callbacks.
//                     Call this at the end of your OnInit() after creating and
//                     embedding the platform-specific window and viewer.
//   - OnUpdate()    — closes the application when IWindow::IsOpen() is false.
//   - OnShutdown()  — logs the shutdown message.
//
// Ownership of the window and viewer stays with the subclass; this base holds
// only non-owning pointers.
class ViewerApplication : public Application
{
public:
    using Application::Application;

protected:
    // Call from OnInit() after creating the window and viewer.
    // Stores non-owning pointers for OnUpdate(); wires setEventCallback and
    // the standard viewer logging AddEventCallback.
    void wireViewer(IWindow* window, IViewerWidget* viewer);

    void OnUpdate()   override;
    void OnShutdown() override;

private:
    IWindow*       m_window = nullptr;
    IViewerWidget* m_viewer = nullptr;
};

} // namespace CellForge
