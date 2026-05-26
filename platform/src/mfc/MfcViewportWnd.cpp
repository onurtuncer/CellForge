// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#include <CellForge/mfc/MfcViewportWnd.h>
#include <CellForge/mfc/MfcKeyMapper.h>

#include <CellForge/Event/KeyboardEvents.h>
#include <CellForge/Event/MouseEvents.h>
#include <CellForge/Event/ApplicationEvent.h>

namespace CellForge {

// ── Message map ───────────────────────────────────────────────────────────────

BEGIN_MESSAGE_MAP(MfcViewportWnd, CWnd)
    ON_WM_KEYDOWN()
    ON_WM_KEYUP()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_RBUTTONDOWN()
    ON_WM_RBUTTONUP()
    ON_WM_MBUTTONDOWN()
    ON_WM_MBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSEWHEEL()
    ON_WM_SIZE()
END_MESSAGE_MAP()

// ── Public ────────────────────────────────────────────────────────────────────

bool MfcViewportWnd::create(CWnd* pParent, const RECT& rect)
{
    // CS_OWNDC: one device context per window — required for future OpenGL/D3D.
    LPCTSTR cls = AfxRegisterWndClass(
        CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
        ::LoadCursor(nullptr, IDC_CROSS),
        static_cast<HBRUSH>(::GetStockObject(BLACK_BRUSH)),
        nullptr);

    return CWnd::Create(cls, _T("CFViewport"),
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        rect, pParent, /* child id */ 1u) == TRUE;
}

void MfcViewportWnd::setEventCallback(std::function<void(Event&)> cb)
{
    m_EventQueue.setCallback(std::move(cb));
}

void MfcViewportWnd::resize(int cx, int cy)
{
    if (m_hWnd)
        ::MoveWindow(m_hWnd, 0, 0, cx, cy, TRUE);
}

// ── Keyboard ──────────────────────────────────────────────────────────────────

void MfcViewportWnd::OnKeyDown(UINT nChar, UINT /*nRepCnt*/, UINT nFlags)
{
    // KF_REPEAT (0x4000 = bit 14 of HIWORD(lParam)) is set on auto-repeat.
    bool repeat = (nFlags & KF_REPEAT) != 0;
    KeyPressedEvent e(MfcKeyMapper::mapKey(nChar), repeat);
    m_EventQueue.dispatch(e);
}

void MfcViewportWnd::OnKeyUp(UINT nChar, UINT /*nRepCnt*/, UINT /*nFlags*/)
{
    KeyReleasedEvent e(MfcKeyMapper::mapKey(nChar));
    m_EventQueue.dispatch(e);
}

// ── Mouse buttons ─────────────────────────────────────────────────────────────

void MfcViewportWnd::OnLButtonDown(UINT /*nFlags*/, CPoint pt)
{
    SetFocus();
    MouseButtonPressedEvent e(MouseButton::Left,
        static_cast<float>(pt.x), static_cast<float>(pt.y));
    m_EventQueue.dispatch(e);
}

void MfcViewportWnd::OnLButtonUp(UINT /*nFlags*/, CPoint pt)
{
    MouseButtonReleasedEvent e(MouseButton::Left,
        static_cast<float>(pt.x), static_cast<float>(pt.y));
    m_EventQueue.dispatch(e);
}

void MfcViewportWnd::OnRButtonDown(UINT /*nFlags*/, CPoint pt)
{
    SetFocus();
    MouseButtonPressedEvent e(MouseButton::Right,
        static_cast<float>(pt.x), static_cast<float>(pt.y));
    m_EventQueue.dispatch(e);
}

void MfcViewportWnd::OnRButtonUp(UINT /*nFlags*/, CPoint pt)
{
    MouseButtonReleasedEvent e(MouseButton::Right,
        static_cast<float>(pt.x), static_cast<float>(pt.y));
    m_EventQueue.dispatch(e);
}

void MfcViewportWnd::OnMButtonDown(UINT /*nFlags*/, CPoint pt)
{
    SetFocus();
    MouseButtonPressedEvent e(MouseButton::Middle,
        static_cast<float>(pt.x), static_cast<float>(pt.y));
    m_EventQueue.dispatch(e);
}

void MfcViewportWnd::OnMButtonUp(UINT /*nFlags*/, CPoint pt)
{
    MouseButtonReleasedEvent e(MouseButton::Middle,
        static_cast<float>(pt.x), static_cast<float>(pt.y));
    m_EventQueue.dispatch(e);
}

// ── Mouse motion / scroll ─────────────────────────────────────────────────────

void MfcViewportWnd::OnMouseMove(UINT /*nFlags*/, CPoint pt)
{
    MouseMovedEvent e(static_cast<float>(pt.x), static_cast<float>(pt.y));
    m_EventQueue.dispatch(e);
}

BOOL MfcViewportWnd::OnMouseWheel(UINT /*nFlags*/, short zDelta, CPoint /*pt*/)
{
    // zDelta: +120 per notch forward, -120 backward. Divide by WHEEL_DELTA
    // to match Qt's 1.0-per-notch convention.
    MouseScrolledEvent e(0.0f, static_cast<float>(zDelta) / WHEEL_DELTA);
    m_EventQueue.dispatch(e);
    return TRUE;
}

// ── Window ────────────────────────────────────────────────────────────────────

void MfcViewportWnd::OnSize(UINT /*nType*/, int cx, int cy)
{
    CWnd::OnSize(0, cx, cy);
    WindowResizeEvent e(static_cast<uint32_t>(cx), static_cast<uint32_t>(cy));
    m_EventQueue.dispatch(e);
}

} // namespace CellForge
