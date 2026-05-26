// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#include "ViewerWidget.h"

#include <QMouseEvent>

namespace CellForge {

ViewerWidget::ViewerWidget(QWidget* parent)
: QWidget(parent)
{
  setAttribute(Qt::WA_PaintOnScreen);
  setAttribute(Qt::WA_NoSystemBackground);
  setMouseTracking(true);
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
}

void ViewerWidget::mouseReleaseEvent(QMouseEvent* theEvent)
{
  QPoint newPoint = theEvent->pos();
  m_viewer->drawPoint(newPoint);
  if (!m_startP.isNull())
    m_viewer->drawLine(m_startP, newPoint);
  m_startP = newPoint;
}

void ViewerWidget::mouseDoubleClickEvent(QMouseEvent* /*event*/)
{
  m_startDrawing = false;
}

} // namespace CellForge
