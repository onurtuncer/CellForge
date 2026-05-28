// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#include <CellForge/IWindow.h>

#ifndef _AFXDLL
#define _AFXDLL
#endif
#include <afxwin.h>

#include <memory>

namespace CellForge {

class IViewportWidget;

// MFC top-level overlapped window implementing IWindow.
// Analogous to QtWindow on the Qt platform.
// Forwards WM_SIZE to the registered IViewportWidget so the child fills the client area.
// WM_CLOSE destroys the window; IWindow::IsOpen() turns false and
// ViewerApplication::OnUpdate() detects the change and calls Application::Close().
class MfcWindow : public CWnd, public IWindow {
    DECLARE_MESSAGE_MAP()

public:
    MfcWindow();
    ~MfcWindow() override;

    bool create(const TCHAR* title, int w, int h);

    // IWindow
    void  Show()  override;
    void  Close() override;
    bool  IsOpen() const override;
    void  SetTitle(const std::string& title) override;
    void  SetSize(int width, int height) override;
    void  SetCentralViewport(std::shared_ptr<IViewportWidget> viewport) override;
    void* NativeHandle() override;

protected:
    afx_msg void OnClose();
    afx_msg void OnSize(UINT nType, int cx, int cy);

private:
    std::shared_ptr<IViewportWidget> m_viewport;
};

} // namespace CellForge
