// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#include <CellForge/mfc/MfcWindow.h>
#include <CellForge/IViewPortWidget.h>

#include <string>

namespace CellForge {

BEGIN_MESSAGE_MAP(MfcWindow, CWnd)
    ON_WM_CLOSE()
    ON_WM_SIZE()
END_MESSAGE_MAP()

MfcWindow::MfcWindow()  = default;
MfcWindow::~MfcWindow() = default;

bool MfcWindow::create(const TCHAR* title, int w, int h)
{
    LPCTSTR cls = AfxRegisterWndClass(
        CS_HREDRAW | CS_VREDRAW,
        ::LoadCursor(nullptr, IDC_ARROW),
        reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1),
        nullptr);
    return CWnd::CreateEx(0, cls, title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, w, h,
        nullptr, nullptr) == TRUE;
}

void MfcWindow::Show()
{
    ShowWindow(SW_SHOW);
    UpdateWindow();
}

void MfcWindow::Close()
{
    if (m_hWnd)
        DestroyWindow();
}

bool MfcWindow::IsOpen() const
{
    return m_hWnd != nullptr;
}

void MfcWindow::SetTitle(const std::string& title)
{
    const std::wstring w(title.begin(), title.end());
    SetWindowText(w.c_str());  // CWnd::SetWindowText — LPCWSTR with UNICODE build
}

void MfcWindow::SetSize(int width, int height)
{
    SetWindowPos(nullptr, 0, 0, width, height,
        SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void MfcWindow::SetCentralViewport(std::shared_ptr<IViewportWidget> viewport)
{
    m_viewport = std::move(viewport);
}

void* MfcWindow::NativeHandle()
{
    return static_cast<void*>(m_hWnd);
}

void MfcWindow::OnClose()
{
    DestroyWindow();   // IsOpen() → false; ViewerApplication::OnUpdate() calls Close()
}

void MfcWindow::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);
    if (m_viewport)
        m_viewport->resize(cx, cy);
}

} // namespace CellForge
