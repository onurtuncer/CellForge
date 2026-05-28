// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

// Demonstrates ViewerApplication with the MFC backend.
// Structurally identical to the Qt viewer example — the only difference is
// the concrete window and viewer types created in OnInit().

#include <CellForge/ViewerApplication.h>
#include <CellForge/EntryPoint.h>
#include <CellForge/Log.h>
#include <CellForge/mfc/MfcApplicationPlatform.h>
#include <CellForge/mfc/MfcWindow.h>
#include <CellForge/mfc/MfcViewerWnd.h>

#ifndef _AFXDLL
#define _AFXDLL
#endif
#include <afxwin.h>

namespace CellForge {

class ViewerApp : public ViewerApplication {
public:
    ViewerApp()
        : ViewerApplication({"CellForge MFC Viewer"},
                            std::make_unique<MfcApplicationPlatform>())
    {}

    void OnInit() override
    {
        m_window = std::make_unique<MfcWindow>();
        if (!m_window->create(_T("CellForge \x2014 MFC Viewer Example"), 900, 600))
        {
            CF_CORE_ERROR("Failed to create main window.");
            Close();
            return;
        }

        CRect clientRect;
        m_window->GetClientRect(&clientRect);

        m_viewer = std::make_unique<MfcViewerWnd>();
        if (!m_viewer->create(m_window.get(), clientRect))
        {
            CF_CORE_ERROR("Failed to create viewer window.");
            Close();
            return;
        }

        // Register viewer for resize propagation from MfcWindow::OnSize.
        // No-op deleter: m_viewer is uniquely owned here; MfcWindow only
        // needs the pointer to forward resize events.
        m_window->SetCentralViewport(
            std::shared_ptr<IViewportWidget>(m_viewer.get(), [](IViewportWidget*){}));

        wireViewer(m_window.get(), m_viewer.get());

        m_window->Show();
        m_viewer->SetFocus();

        CF_CORE_INFO("MFC Viewer running — click to place points, double-click to restart.");
    }

    void OnShutdown() override
    {
        ViewerApplication::OnShutdown();
        m_viewer.reset();
        m_window.reset();
    }

private:
    std::unique_ptr<MfcWindow>    m_window;
    std::unique_ptr<MfcViewerWnd> m_viewer;
};

Application* CreateApplication(int /*argc*/, char** /*argv*/)
{
    return new ViewerApp();
}

} // namespace CellForge
