// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#include <CellForge/Event/Event.h>
#include <CellForge/IViewerWidget.h>

#ifndef _AFXDLL
#define _AFXDLL
#endif
#include <afxwin.h>

#include <functional>
#include <memory>

namespace CellForge {

class Viewer;

// MFC child window that hosts an OCCT Viewer and translates Win32 input
// messages into typed CellForge events.  Analogous to ViewerWidget on Qt.
class MfcViewerWnd : public CWnd, public IViewerWidget {
    DECLARE_MESSAGE_MAP()

public:
    MfcViewerWnd();
    ~MfcViewerWnd() override;

    bool create(CWnd* pParent, const RECT& rect);

    // IViewerWidget / IViewportWidget
    void* NativeHandle() override { return static_cast<void*>(m_hWnd); }
    void  resize(int cx, int cy) override;

    using EventCallbackFn = std::function<void(Event&)>;
    void setEventCallback(EventCallbackFn cb) override;

protected:
    afx_msg int  OnCreate(LPCREATESTRUCT lpcs);
    afx_msg void OnPaint();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint pt);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint pt);

private:
    std::unique_ptr<Viewer> m_viewer;
    EventCallbackFn         m_callback;
    bool                    m_hasStartPoint = false;
    int                     m_startX        = 0;
    int                     m_startY        = 0;
};

} // namespace CellForge
