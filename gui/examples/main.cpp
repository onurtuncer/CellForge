// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

// Demonstrates viewer capabilities:
//   - Multiple OCCT shapes displayed at once
//   - Fit-all on startup
//   - Orbit (left drag), pan (middle drag), zoom (scroll wheel)
//   - Shading toggle via S / W keys
//   - Standard view directions via T / B / L / R / Backspace keys
//   - Custom background colour

#include <CellForge/ViewerApplication.h>
#include <CellForge/EntryPoint.h>
#include <CellForge/Log.h>
#include <CellForge/qt/QtApplicationPlatform.h>
#include <CellForge/qt/QtWindow.h>
#include <CellForge/qt/widgets/ViewerWidget.h>

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
    ViewerApp(int argc, char** argv)
        : ViewerApplication({"CellForge Viewer"},
                            std::make_unique<QtApplicationPlatform>(argc, argv))
    {}

    void OnInit() override
    {
        m_qtWindow = std::make_unique<QtWindow>("CellForge \xe2\x80\x94 Qt Viewer", 1024, 720);

        m_viewer = new ViewerWidget(m_qtWindow->qtMainWindow());
        m_qtWindow->qtMainWindow()->setCentralWidget(m_viewer);
        m_qtWindow->Show();

        wireViewer(m_qtWindow.get(), m_viewer);

        m_viewer->setBackgroundColor(Quantity_Color(0.13, 0.13, 0.15, Quantity_TOC_sRGB));
        m_viewer->displayShapes(makeDemoShapes());
        m_viewer->fitAll();

        CF_CORE_INFO("Qt Viewer ready.");
        CF_CORE_INFO("  Orbit: left-drag  |  Pan: middle-drag  |  Zoom: scroll wheel");
        CF_CORE_INFO("  Fit: F  |  Shaded: S  |  Wireframe: W");
        CF_CORE_INFO("  Views: T=top  B=bottom  L=left  R=right  Backspace=iso");
    }

    void OnShutdown() override
    {
        ViewerApplication::OnShutdown();
        m_viewer = nullptr;
        m_qtWindow.reset();
    }

private:
    std::unique_ptr<QtWindow> m_qtWindow;
    ViewerWidget*             m_viewer = nullptr;
};

Application* CreateApplication(int argc, char** argv)
{
    return new ViewerApp(argc, argv);
}

} // namespace CellForge
