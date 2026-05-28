// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

// Demonstrates viewer capabilities with the MFC backend.
// Structurally identical to the Qt viewer example — the only difference is
// the concrete window and viewer types created in OnInit().

// afxwin.h must be the first include in every MFC translation unit so that
// Windows.h is pulled in fully (with NLS, user32, etc.) before OCCT or spdlog
// can include it in a restricted mode.
#ifndef _AFXDLL
#define _AFXDLL
#endif
#include <afxwin.h>

#include <CellForge/ViewerApplication.h>
#include <CellForge/EntryPoint.h>
#include <CellForge/Log.h>
#include <CellForge/mfc/MfcApplicationPlatform.h>
#include <CellForge/mfc/MfcWindow.h>
#include <CellForge/mfc/MfcViewerWnd.h>

#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <Quantity_Color.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

namespace CellForge {

static std::vector<TopoDS_Shape> makeDemoShapes()
{
    const gp_Dir zDir(0.0, 0.0, 1.0);

    TopoDS_Shape box = BRepPrimAPI_MakeBox(
        gp_Pnt(-5.0, -1.0, 0.0), 2.0, 2.0, 2.0).Shape();

    TopoDS_Shape sphere = BRepPrimAPI_MakeSphere(
        gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), zDir), 1.5).Shape();

    TopoDS_Shape cylinder = BRepPrimAPI_MakeCylinder(
        gp_Ax2(gp_Pnt(3.5, -1.0, 0.0), zDir), 1.0, 3.0).Shape();

    return { box, sphere, cylinder };
}

class ViewerApp : public ViewerApplication {
public:
    ViewerApp()
        : ViewerApplication({"CellForge MFC Viewer"},
                            std::make_unique<MfcApplicationPlatform>())
    {}

    void OnInit() override
    {
        m_window = std::make_unique<MfcWindow>();
        if (!m_window->create(_T("CellForge \x2014 MFC Viewer"), 1024, 720))
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

        m_window->SetCentralViewport(
            std::shared_ptr<IViewportWidget>(m_viewer.get(), [](IViewportWidget*){}));

        wireViewer(m_window.get(), m_viewer.get());

        m_viewer->setBackgroundColor(Quantity_Color(0.13, 0.13, 0.15, Quantity_TOC_sRGB));
        m_viewer->displayShapes(makeDemoShapes());
        m_viewer->fitAll();

        m_window->Show();
        m_viewer->SetFocus();

        CF_CORE_INFO("MFC Viewer ready.");
        CF_CORE_INFO("  Orbit: left-drag  |  Pan: middle-drag  |  Zoom: scroll wheel");
        CF_CORE_INFO("  Fit: F  |  Shaded: S  |  Wireframe: W");
        CF_CORE_INFO("  Views: T=top  B=bottom  L=left  R=right  Backspace=iso");
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
