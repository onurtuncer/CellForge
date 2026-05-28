// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#include <CellForge/qt/widgets/ViewerWidget.h>
#include <CellForge/qt/input/QtEventMapper.h>

#include <CellForge/Event/MouseEvents.h>
#include <CellForge/Event/ApplicationEvent.h>
#include <CellForge/Viewer.h>

#include <QMouseEvent>
#include <QResizeEvent>

#include <Aspect_Handle.hxx>

namespace CellForge {

ViewerWidget::ViewerWidget(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    setMouseTracking(true);
}

ViewerWidget::~ViewerWidget()
{
    delete m_viewer;
}

void ViewerWidget::setEventCallback(EventCallbackFn cb)
{
    m_callback = std::move(cb);
}

void ViewerWidget::ensureViewer()
{
    if (!m_isInitialized) {
        m_viewer        = new Viewer((Aspect_Handle) winId());
        m_isInitialized = true;
    }
}

void ViewerWidget::paintEvent(QPaintEvent* /*event*/)
{
    ensureViewer();
    m_viewer->redrawView();
}

void ViewerWidget::resizeEvent(QResizeEvent* event)
{
    ensureViewer();
    m_viewer->resizeView();

    if (m_callback) {
        WindowResizeEvent e(static_cast<uint32_t>(event->size().width()),
                            static_cast<uint32_t>(event->size().height()));
        m_callback(e);
    }
    QWidget::resizeEvent(event);
}

void ViewerWidget::mouseReleaseEvent(QMouseEvent* event)
{
    ensureViewer();
    const int nx = event->pos().x();
    const int ny = event->pos().y();

    m_viewer->drawPoint(nx, ny);
    if (m_hasStartPoint)
        m_viewer->drawLine(m_startX, m_startY, nx, ny);
    m_hasStartPoint = true;
    m_startX = nx;
    m_startY = ny;

    if (m_callback) {
        MouseButtonReleasedEvent e(QtEventMapper::mapMouseButton(event->button()),
                                   static_cast<float>(nx),
                                   static_cast<float>(ny));
        m_callback(e);
    }
}

void ViewerWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    m_hasStartPoint = false;

    if (m_callback) {
        MouseButtonPressedEvent e(QtEventMapper::mapMouseButton(event->button()),
                                  static_cast<float>(event->pos().x()),
                                  static_cast<float>(event->pos().y()));
        m_callback(e);
    }
}

} // namespace CellForge
