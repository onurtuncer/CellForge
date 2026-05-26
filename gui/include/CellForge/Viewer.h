// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#ifdef _WIN32
#  include <Windows.h>
#endif

#include <TopoDS_Shape.hxx>
#include <WNT_Window.hxx>

#include <vector>

class V3d_Viewer;
class V3d_View;
class AIS_InteractiveContext;
class AIS_ViewController;
class QPoint;

namespace CellForge {

class Viewer
{
public:
  explicit Viewer(Aspect_Handle windowHandle);

  void redrawView();
  void resizeView();

  void drawPoint(const QPoint& p);
  void drawLine(const QPoint& p1, const QPoint& p2);

private:
  void init(const HANDLE& windowHandle);

  Handle(V3d_Viewer)             m_viewer;
  Handle(V3d_View)               m_view;
  Handle(AIS_InteractiveContext) m_context;
};

} // namespace CellForge
