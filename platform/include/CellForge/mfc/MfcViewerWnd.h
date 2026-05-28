// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#ifndef _AFXDLL
#define _AFXDLL
#endif
#include <afxwin.h>

#include <CellForge/Event/Event.h>
#include <CellForge/IViewerWidget.h>

#include <functional>
#include <memory>

namespace CellForge {

class Viewer;

// MFC child window hosting an OCCT 3D view.
// Mouse navigation (orbit = left drag, pan = middle drag, zoom = wheel) and
// keyboard shortcuts (F = fit-all, S/W = shaded/wireframe, T/B/L/R =
// standard views) are handled by the OCCT AIS_ViewController pipeline.
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

    // Shape display API
    void displayShapes(const std::vector<TopoDS_Shape>& shapes) override;
    void clearShapes()                                           override;
    void fitAll()                                                override;
    void setBackgroundColor(const Quantity_Color& color)         override;
    void setDisplayMode(AIS_DisplayMode mode)                    override;

protected:
    afx_msg int  OnCreate(LPCREATESTRUCT lpcs);
    afx_msg void OnPaint();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnMouseMove(UINT nFlags, CPoint pt);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint pt);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint pt);
    afx_msg void OnMButtonDown(UINT nFlags, CPoint pt);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint pt);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint pt);
    afx_msg void OnMButtonUp(UINT nFlags, CPoint pt);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

private:
    void handleMouseDown(UINT nFlags, CPoint pt);
    void handleMouseUp(UINT nFlags, CPoint pt);

    std::unique_ptr<Viewer> m_viewer;
    EventCallbackFn         m_callback;
};

} // namespace CellForge
