// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#ifdef CF_PLATFORM_WINDOWS
#  include <Windows.h>
#endif

#include <Aspect_Handle.hxx>
#include <AIS_DisplayMode.hxx>
#include <AIS_Shape.hxx>
#include <Quantity_Color.hxx>
#include <TopoDS_Shape.hxx>

#include <CellForge/ViewerInteractor.h>

#include <vector>

class V3d_Viewer;
class V3d_View;
class AIS_InteractiveContext;

namespace CellForge {

class Viewer
{
public:
  explicit Viewer(Aspect_Handle windowHandle);

  void redrawView();
  void resizeView();

  // Shape display
  void displayShapes(const std::vector<TopoDS_Shape>& shapes);
  void clearShapes();

  // View controls
  void fitAll();
  void setBackgroundColor(const Quantity_Color& color);
  void setDisplayMode(AIS_DisplayMode mode);

  // Access the OCCT interaction controller (orbit / pan / zoom / key dispatch).
  ViewerInteractor* interactor() const;

  // Legacy helpers: place a point or line at pixel-space coordinates.
  void drawPoint(int px, int py);
  void drawLine(int x1, int y1, int x2, int y2);

private:
  void init(Aspect_Handle windowHandle);

  Handle(V3d_Viewer)             m_viewer;
  Handle(V3d_View)               m_view;
  Handle(AIS_InteractiveContext) m_context;
  Handle(ViewerInteractor)       m_interactor;
  std::vector<Handle(AIS_Shape)> m_aisShapes;
};

} // namespace CellForge
