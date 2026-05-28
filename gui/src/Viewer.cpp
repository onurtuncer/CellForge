// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#include "Viewer.h"

#include <AIS_InteractiveContext.hxx>
#include <AIS_Line.hxx>
#include <AIS_Point.hxx>
#include <AIS_Shape.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <Aspect_Handle.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Line.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <V3d_AmbientLight.hxx>
#include <V3d_DirectionalLight.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <WNT_Window.hxx>

namespace CellForge {

Viewer::Viewer(Aspect_Handle windowHandle)
{
  init(windowHandle);
}

void Viewer::init(Aspect_Handle windowHandle)
{
  static Handle(Aspect_DisplayConnection) displayConnection;
  if (displayConnection.IsNull())
    displayConnection = new Aspect_DisplayConnection();

  HWND winHandle = (HWND) windowHandle;
  if (winHandle == NULL)
    return;

  Handle(OpenGl_GraphicDriver) graphicDriver =
      new OpenGl_GraphicDriver(displayConnection, false);

  m_viewer = new V3d_Viewer(graphicDriver);

  Handle(V3d_DirectionalLight) lightDir =
      new V3d_DirectionalLight(V3d_Zneg, Quantity_Color(Quantity_NOC_GRAY97), 1);
  Handle(V3d_AmbientLight) lightAmb = new V3d_AmbientLight();
  lightDir->SetDirection(1.0, -2.0, -10.0);
  m_viewer->AddLight(lightDir);
  m_viewer->AddLight(lightAmb);
  m_viewer->SetLightOn(lightDir);
  m_viewer->SetLightOn(lightAmb);

  m_context = new AIS_InteractiveContext(m_viewer);

  const Handle(Prs3d_Drawer)& drawer = m_context->DefaultDrawer();
  if (!drawer.IsNull())
  {
    const Handle(Prs3d_ShadingAspect)&        sa = drawer->ShadingAspect();
    const Handle(Graphic3d_AspectFillArea3d)& fa = sa->Aspect();
    drawer->SetFaceBoundaryDraw(true);
    fa->SetEdgeOff();
    drawer->SetMaximalParameterValue(1000);
  }

  m_view = m_viewer->CreateView();
  m_view->SetImmediateUpdate(false);
  m_view->SetBackgroundColor(Quantity_NOC_BLACK);

  Handle(Aspect_Window) wnd = new WNT_Window(windowHandle);
  m_view->SetWindow(wnd, nullptr);
  if (!wnd->IsMapped())
    wnd->Map();
  m_view->MustBeResized();

  m_view->SetShadingModel(V3d_PHONG);

  Graphic3d_RenderingParams& rp = m_view->ChangeRenderingParams();
  rp.IsAntialiasingEnabled = true;
  rp.NbMsaaSamples         = 8;
  rp.IsShadowEnabled       = false;
  rp.CollectedStats         = Graphic3d_RenderingParams::PerfCounters_NONE;

  m_interactor = new ViewerInteractor(m_view, m_context);
}

void Viewer::redrawView()
{
  m_view->Redraw();
}

void Viewer::resizeView()
{
  m_view->MustBeResized();
}

void Viewer::displayShapes(const std::vector<TopoDS_Shape>& shapes)
{
  clearShapes();
  m_aisShapes.reserve(shapes.size());
  for (const TopoDS_Shape& shape : shapes)
  {
    Handle(AIS_Shape) ais = new AIS_Shape(shape);
    m_aisShapes.push_back(ais);
    m_context->Display(ais, false);
  }
  m_context->UpdateCurrentViewer();
}

void Viewer::clearShapes()
{
  for (const Handle(AIS_Shape)& ais : m_aisShapes)
    m_context->Remove(ais, false);
  m_aisShapes.clear();
  m_context->UpdateCurrentViewer();
}

void Viewer::fitAll()
{
  m_view->FitAll(0.01, true);
}

void Viewer::setBackgroundColor(const Quantity_Color& color)
{
  m_view->SetBackgroundColor(color);
  m_view->Redraw();
}

void Viewer::setDisplayMode(AIS_DisplayMode mode)
{
  m_context->SetDisplayMode(static_cast<Standard_Integer>(mode), false);
  m_context->UpdateCurrentViewer();
}

ViewerInteractor* Viewer::interactor() const
{
  return m_interactor.get();
}

void Viewer::drawPoint(int px, int py)
{
  double x, y, z;
  m_view->Convert(px, py, x, y, z);
  Handle(Geom_Point) gp  = new Geom_CartesianPoint(gp_Pnt(x, y, z));
  Handle(AIS_Point)  prs = new AIS_Point(gp);
  m_context->Display(prs, true);
}

void Viewer::drawLine(int x1, int y1, int x2, int y2)
{
  double x, y, z;
  m_view->Convert(x1, y1, x, y, z);
  Handle(Geom_Point) gp1 = new Geom_CartesianPoint(gp_Pnt(x, y, z));
  m_view->Convert(x2, y2, x, y, z);
  Handle(Geom_Point) gp2 = new Geom_CartesianPoint(gp_Pnt(x, y, z));
  Handle(AIS_Line)   prs = new AIS_Line(gp1, gp2);
  m_context->Display(prs, true);
}

} // namespace CellForge
