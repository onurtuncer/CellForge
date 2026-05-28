// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#include <CellForge/qt/widgets/ViewPortWidget.h>
#include <CellForge/qt/input/QtEventMapper.h>

#include <CellForge/Event/KeyboardEvents.h>
#include <CellForge/Event/MouseEvents.h>
#include <CellForge/Event/ApplicationEvent.h>

#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QCloseEvent>

namespace CellForge {

ViewportWidget::ViewportWidget(QWidget* parent)
    : QWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
}

void ViewportWidget::resize(int cx, int cy)
{
    QWidget::resize(cx, cy);
}

void ViewportWidget::setEventCallback(std::function<void(Event&)> cb)
{
    m_EventQueue.setCallback(std::move(cb));
}

void ViewportWidget::keyPressEvent(QKeyEvent* event)
{
    KeyCode code = QtEventMapper::mapKey(event->key());
    KeyPressedEvent e(code, event->isAutoRepeat());
    m_EventQueue.dispatch(e);
}

void ViewportWidget::keyReleaseEvent(QKeyEvent* event)
{
    // Qt fires release events for auto-repeat too; suppress them.
    if (event->isAutoRepeat())
        return;
    KeyCode code = QtEventMapper::mapKey(event->key());
    KeyReleasedEvent e(code);
    m_EventQueue.dispatch(e);
}

void ViewportWidget::mousePressEvent(QMouseEvent* event)
{
    MouseButton btn = QtEventMapper::mapMouseButton(event->button());
    MouseButtonPressedEvent e(btn,
        static_cast<float>(event->pos().x()),
        static_cast<float>(event->pos().y()));
    m_EventQueue.dispatch(e);
}

void ViewportWidget::mouseReleaseEvent(QMouseEvent* event)
{
    MouseButton btn = QtEventMapper::mapMouseButton(event->button());
    MouseButtonReleasedEvent e(btn,
        static_cast<float>(event->pos().x()),
        static_cast<float>(event->pos().y()));
    m_EventQueue.dispatch(e);
}

void ViewportWidget::mouseMoveEvent(QMouseEvent* event)
{
    MouseMovedEvent e(
        static_cast<float>(event->pos().x()),
        static_cast<float>(event->pos().y()));
    m_EventQueue.dispatch(e);
}

void ViewportWidget::wheelEvent(QWheelEvent* event)
{
    // angleDelta() is in eighths of a degree; one notch = 120 units = 1.0 scroll step.
    QPoint delta = event->angleDelta();
    MouseScrolledEvent e(
        static_cast<float>(delta.x()) / 120.0f,
        static_cast<float>(delta.y()) / 120.0f);
    m_EventQueue.dispatch(e);
}

void ViewportWidget::resizeEvent(QResizeEvent* event)
{
    WindowResizeEvent e(
        static_cast<uint32_t>(event->size().width()),
        static_cast<uint32_t>(event->size().height()));
    m_EventQueue.dispatch(e);
    QWidget::resizeEvent(event);
}

void ViewportWidget::closeEvent(QCloseEvent* event)
{
    WindowCloseEvent e;
    m_EventQueue.dispatch(e);
    QWidget::closeEvent(event);
}

} // namespace CellForge
