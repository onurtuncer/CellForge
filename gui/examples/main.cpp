// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

// Demonstrates ViewerApplication with the Qt backend.
// OnInit() creates a QtWindow + ViewerWidget, wires them via wireViewer(), and
// returns.  OnUpdate() / OnShutdown() are handled by ViewerApplication.

#include <CellForge/ViewerApplication.h>
#include <CellForge/EntryPoint.h>
#include <CellForge/Log.h>
#include <CellForge/qt/QtApplicationPlatform.h>
#include <CellForge/qt/QtWindow.h>
#include <CellForge/qt/widgets/ViewerWidget.h>

namespace CellForge {

class ViewerApp : public ViewerApplication {
public:
    ViewerApp(int argc, char** argv)
        : ViewerApplication({"CellForge Viewer"},
                            std::make_unique<QtApplicationPlatform>(argc, argv))
    {}

    void OnInit() override
    {
        m_qtWindow = std::make_unique<QtWindow>("CellForge — Qt Viewer Example", 900, 600);

        // ViewerWidget parented to the QMainWindow so Qt owns and destroys it.
        m_viewer = new ViewerWidget(m_qtWindow->qtMainWindow());
        m_qtWindow->qtMainWindow()->setCentralWidget(m_viewer);

        m_qtWindow->Show();
        wireViewer(m_qtWindow.get(), m_viewer);

        CF_CORE_INFO("Qt Viewer running — click to place points, double-click to restart.");
    }

    void OnShutdown() override
    {
        ViewerApplication::OnShutdown();
        m_viewer = nullptr;        // Qt-owned; destroyed with the window
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
