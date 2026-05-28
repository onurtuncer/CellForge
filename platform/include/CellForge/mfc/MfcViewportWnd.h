// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#include <CellForge/Event/EventQueue.h>
#include <CellForge/IViewPortWidget.h>

#ifndef _AFXDLL
#define _AFXDLL
#endif
#include <afxwin.h>

#include <functional>

namespace CellForge {

// MFC child window that translates Win32 input/window messages into typed
// CellForge events and forwards them via EventQueue::dispatch().
// Analogous to ViewportWidget on the Qt platform.
class MfcViewportWnd : public CWnd, public IViewportWidget {
    DECLARE_MESSAGE_MAP()

public:
    MfcViewportWnd()  = default;
    ~MfcViewportWnd() override = default;

    // Creates the child window. Call this from Application::OnInit() after
    // the parent window exists.  The window fills the entire client area of
    // pParent; call resize() from the parent's WM_SIZE handler to keep it in sync.
    bool create(CWnd* pParent, const RECT& rect);

    // IViewportWidget
    void* NativeHandle() override { return static_cast<void*>(m_hWnd); }
    void  resize(int cx, int cy) override;
    void  setEventCallback(std::function<void(Event&)> cb) override;

protected:
    // ── Keyboard ──────────────────────────────────────────────────────────
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyUp  (UINT nChar, UINT nRepCnt, UINT nFlags);

    // ── Mouse buttons ─────────────────────────────────────────────────────
    afx_msg void OnLButtonDown(UINT nFlags, CPoint pt);
    afx_msg void OnLButtonUp  (UINT nFlags, CPoint pt);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint pt);
    afx_msg void OnRButtonUp  (UINT nFlags, CPoint pt);
    afx_msg void OnMButtonDown(UINT nFlags, CPoint pt);
    afx_msg void OnMButtonUp  (UINT nFlags, CPoint pt);

    // ── Mouse motion / scroll ─────────────────────────────────────────────
    afx_msg void OnMouseMove (UINT nFlags, CPoint pt);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

    // ── Window ────────────────────────────────────────────────────────────
    afx_msg void OnSize(UINT nType, int cx, int cy);

private:
    EventQueue m_EventQueue;
};

} // namespace CellForge
