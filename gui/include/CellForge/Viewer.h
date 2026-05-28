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

#include <Aspect_Handle.hxx>
#include <TopoDS_Shape.hxx>

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

  // Pixel-space coordinates; the viewer projects them into 3-D space internally.
  void drawPoint(int px, int py);
  void drawLine(int x1, int y1, int x2, int y2);

private:
  void init(Aspect_Handle windowHandle);

  Handle(V3d_Viewer)             m_viewer;
  Handle(V3d_View)               m_view;
  Handle(AIS_InteractiveContext) m_context;
};

} // namespace CellForge
