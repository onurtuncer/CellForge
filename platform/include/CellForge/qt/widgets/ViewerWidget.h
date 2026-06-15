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

#include <AIS_DisplayMode.hxx>
#include <Quantity_Color.hxx>
#include <TopoDS_Shape.hxx>

#include <QElapsedTimer>
#include <QWidget>

#include <functional>
#include <vector>

class QPaintEvent;
class QResizeEvent;
class QMouseEvent;
class QWheelEvent;
class QKeyEvent;

namespace CellForge {

class Viewer;

// Qt widget that hosts an OCCT 3D view.
// Mouse navigation (orbit = left drag, pan = middle drag, zoom = wheel) and
// keyboard shortcuts (F = fit-all, S/W = shaded/wireframe, T/B/L/R =
// standard views) are handled by the OCCT AIS_ViewController pipeline.
// Shape display and view controls are forwarded to the owned Viewer.
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

    // Shape display API
    void displayShapes(const std::vector<TopoDS_Shape>& shapes) override;
    void clearShapes()                                           override;
    void fitAll()                                                override;
    void setBackgroundColor(const Quantity_Color& color)         override;
    void setDisplayMode(AIS_DisplayMode mode)                    override;

private:
    void paintEvent(QPaintEvent*   event) override;
    void resizeEvent(QResizeEvent*  event) override;
    void mousePressEvent(QMouseEvent*   event) override;
    void mouseMoveEvent(QMouseEvent*    event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent*   event) override;
    void keyPressEvent(QKeyEvent*   event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

    void ensureViewer();

    Viewer*         m_viewer        = nullptr;
    bool            m_isInitialized = false;
    QElapsedTimer   m_timer;
    EventCallbackFn m_callback;
};

} // namespace CellForge
