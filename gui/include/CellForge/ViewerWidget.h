// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#include "Viewer.h"

#include <CellForge/Event/Event.h>

#include <QWidget>
#include <functional>

namespace CellForge {

class ViewerWidget : public QWidget
{
  Q_OBJECT

public:
  explicit ViewerWidget(QWidget* parent = nullptr);

  QPaintEngine* paintEngine() const override { return nullptr; }

  using EventCallbackFn = std::function<void(Event&)>;
  void setEventCallback(EventCallbackFn cb);

private:
  void paintEvent(QPaintEvent*  theEvent) override;
  void resizeEvent(QResizeEvent* theEvent) override;
  void mouseReleaseEvent(QMouseEvent* theEvent) override;
  void mouseDoubleClickEvent(QMouseEvent* event) override;

  Viewer* m_viewer        = nullptr;
  bool    m_isInitialized = false;
  bool    m_startDrawing  = false;
  QPoint  m_startP;
  EventCallbackFn m_callback;
};

} // namespace CellForge
