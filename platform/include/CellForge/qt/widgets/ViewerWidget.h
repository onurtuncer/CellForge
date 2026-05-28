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

#include <QWidget>
#include <functional>

class QPaintEvent;
class QResizeEvent;
class QMouseEvent;

namespace CellForge {

class Viewer;

// Qt widget that hosts an OCCT 3D view and translates Qt events into
// CellForge typed events delivered through setEventCallback.
class ViewerWidget : public QWidget, public IViewerWidget
{
    Q_OBJECT

public:
    explicit ViewerWidget(QWidget* parent = nullptr);
    ~ViewerWidget();

    // Suppress Qt's default paint engine — OCCT owns the surface.
    QPaintEngine* paintEngine() const override { return nullptr; }

    // IViewerWidget / IViewportWidget
    void* NativeHandle() override { return static_cast<void*>(this); }
    void  resize(int cx, int cy) override;

    using EventCallbackFn = std::function<void(Event&)>;
    void setEventCallback(EventCallbackFn cb) override;

private:
    // Qt event overrides are private implementation details; the public
    // contract is the CellForge EventCallbackFn above.
    void paintEvent(QPaintEvent*  event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

    void ensureViewer();

    Viewer*         m_viewer        = nullptr;
    bool            m_isInitialized = false;
    bool            m_hasStartPoint = false;
    int             m_startX        = 0;
    int             m_startY        = 0;
    EventCallbackFn m_callback;
};

} // namespace CellForge
