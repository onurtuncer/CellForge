// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#include <CellForge/IViewPortWidget.h>
#include <AIS_DisplayMode.hxx>
#include <Quantity_Color.hxx>
#include <TopoDS_Shape.hxx>

#include <vector>

namespace CellForge {

// Interface for a viewport widget that hosts an OCCT 3D view.
// Extends IViewportWidget so a viewer widget can be passed wherever a generic
// viewport widget is accepted (e.g. IWindow::SetCentralViewport).
// Implemented by ViewerWidget (Qt) and MfcViewerWnd (MFC).
class IViewerWidget : public IViewportWidget
{
public:
  virtual void displayShapes(const std::vector<TopoDS_Shape>& shapes) = 0;
  virtual void clearShapes() = 0;
  virtual void fitAll() = 0;
  virtual void setBackgroundColor(const Quantity_Color& color) = 0;
  virtual void setDisplayMode(AIS_DisplayMode mode) = 0;
};

} // namespace CellForge
