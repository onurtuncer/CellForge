// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#include <CellForge/mfc/MfcViewerWnd.h>

#include <CellForge/Viewer.h>
#include <CellForge/Event/MouseEvents.h>
#include <CellForge/Event/ApplicationEvent.h>

namespace CellForge {

BEGIN_MESSAGE_MAP(MfcViewerWnd, CWnd)
    ON_WM_CREATE()
    ON_WM_PAINT()
    ON_WM_SIZE()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

MfcViewerWnd::MfcViewerWnd() = default;
MfcViewerWnd::~MfcViewerWnd() = default;

bool MfcViewerWnd::create(CWnd* pParent, const RECT& rect)
{
    // CS_OWNDC: dedicated device context required by the OpenGL driver.
    // CS_DBLCLKS: receive WM_LBUTTONDBLCLK; without this the class only
    //             delivers WM_LBUTTONDOWN twice.
    LPCTSTR cls = AfxRegisterWndClass(
        CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS,
        ::LoadCursor(nullptr, IDC_CROSS),
        static_cast<HBRUSH>(::GetStockObject(BLACK_BRUSH)),
        nullptr);

    return CWnd::Create(cls, _T("CFViewerWnd"),
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        rect, pParent, 2u) == TRUE;
}

void MfcViewerWnd::setEventCallback(EventCallbackFn cb)
{
    m_callback = std::move(cb);
}

void MfcViewerWnd::resize(int cx, int cy)
{
    if (m_hWnd)
        ::MoveWindow(m_hWnd, 0, 0, cx, cy, TRUE);
}

// ── Handlers ──────────────────────────────────────────────────────────────────

int MfcViewerWnd::OnCreate(LPCREATESTRUCT lpcs)
{
    if (CWnd::OnCreate(lpcs) == -1)
        return -1;
    m_viewer = std::make_unique<Viewer>((Aspect_Handle) m_hWnd);
    return 0;
}

void MfcViewerWnd::OnPaint()
{
    CPaintDC dc(this);  // validates the update region
    m_viewer->redrawView();
}

void MfcViewerWnd::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);
    if (m_viewer)
        m_viewer->resizeView();

    if (m_callback) {
        WindowResizeEvent e(static_cast<uint32_t>(cx), static_cast<uint32_t>(cy));
        m_callback(e);
    }
}

void MfcViewerWnd::OnLButtonUp(UINT /*nFlags*/, CPoint pt)
{
    if (!m_viewer) return;

    m_viewer->drawPoint(pt.x, pt.y);
    if (m_hasStartPoint)
        m_viewer->drawLine(m_startX, m_startY, pt.x, pt.y);
    m_hasStartPoint = true;
    m_startX = pt.x;
    m_startY = pt.y;

    if (m_callback) {
        MouseButtonReleasedEvent e(MouseButton::Left,
            static_cast<float>(pt.x), static_cast<float>(pt.y));
        m_callback(e);
    }
}

void MfcViewerWnd::OnLButtonDblClk(UINT /*nFlags*/, CPoint pt)
{
    m_hasStartPoint = false;

    if (m_callback) {
        MouseButtonPressedEvent e(MouseButton::Left,
            static_cast<float>(pt.x), static_cast<float>(pt.y));
        m_callback(e);
    }
}

} // namespace CellForge
