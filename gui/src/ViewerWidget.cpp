// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#include "ViewerWidget.h"

#include <CellForge/Event/MouseEvents.h>
#include <CellForge/Event/ApplicationEvent.h>

#include <QMouseEvent>

namespace CellForge {

namespace {

MouseButton qtButtonToMouseButton(Qt::MouseButton btn)
{
  switch (btn) {
    case Qt::RightButton:  return MouseButton::Right;
    case Qt::MiddleButton: return MouseButton::Middle;
    default:               return MouseButton::Left;
  }
}

} // namespace

ViewerWidget::ViewerWidget(QWidget* parent)
: QWidget(parent)
{
  setAttribute(Qt::WA_PaintOnScreen);
  setAttribute(Qt::WA_NoSystemBackground);
  setMouseTracking(true);
}

void ViewerWidget::setEventCallback(EventCallbackFn cb)
{
  m_callback = std::move(cb);
}

void ViewerWidget::paintEvent(QPaintEvent* /*theEvent*/)
{
  if (!m_isInitialized)
  {
    m_viewer        = new Viewer((Aspect_Handle) winId());
    m_isInitialized = true;
  }
  m_viewer->redrawView();
}

void ViewerWidget::resizeEvent(QResizeEvent* /*theEvent*/)
{
  if (!m_isInitialized)
  {
    m_viewer        = new Viewer((Aspect_Handle) winId());
    m_isInitialized = true;
  }
  m_viewer->resizeView();

  if (m_callback) {
    WindowResizeEvent e(static_cast<unsigned>(width()), static_cast<unsigned>(height()));
    m_callback(e);
  }
}

void ViewerWidget::mouseReleaseEvent(QMouseEvent* theEvent)
{
  QPoint newPoint = theEvent->pos();
  m_viewer->drawPoint(newPoint);
  if (!m_startP.isNull())
    m_viewer->drawLine(m_startP, newPoint);
  m_startP = newPoint;

  if (m_callback) {
    MouseButtonReleasedEvent e(qtButtonToMouseButton(theEvent->button()),
                               static_cast<float>(newPoint.x()),
                               static_cast<float>(newPoint.y()));
    m_callback(e);
  }
}

void ViewerWidget::mouseDoubleClickEvent(QMouseEvent* theEvent)
{
  m_startDrawing = false;

  if (m_callback) {
    MouseButtonPressedEvent e(qtButtonToMouseButton(theEvent->button()),
                              static_cast<float>(theEvent->pos().x()),
                              static_cast<float>(theEvent->pos().y()));
    m_callback(e);
  }
}

} // namespace CellForge
