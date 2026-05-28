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

#include <QWidget>
#include <functional>

class QKeyEvent;
class QMouseEvent;
class QWheelEvent;
class QResizeEvent;
class QCloseEvent;

namespace CellForge {

class ViewportWidget : public QWidget, public IViewportWidget {
    Q_OBJECT

public:
    explicit ViewportWidget(QWidget* parent = nullptr);
    ~ViewportWidget() override = default;

    // IViewportWidget
    void* NativeHandle() override { return static_cast<void*>(this); }
    void  resize(int cx, int cy) override;
    void  setEventCallback(std::function<void(Event&)> cb) override;

protected:
    // Names must be camelCase — Qt resolves virtual dispatch by exact name.
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private:
    EventQueue m_EventQueue;
};

} // namespace CellForge
