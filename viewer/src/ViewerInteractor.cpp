// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#include "ViewerInteractor.h"

#include <AIS_DisplayMode.hxx>
#include <AIS_InteractiveContext.hxx>
#include <Aspect_VKeySet.hxx>
#include <V3d_TypeOfOrientation.hxx>
#include <V3d_View.hxx>

namespace CellForge {

ViewerInteractor::ViewerInteractor(const Handle(V3d_View)&               view,
                                   const Handle(AIS_InteractiveContext)& ctx)
: m_view(view), m_ctx(ctx)
{}

ViewerInteractor::~ViewerInteractor() {}

bool ViewerInteractor::UpdateMouseButtons(const NCollection_Vec2<int>& point,
                                          Aspect_VKeyMouse             buttons,
                                          Aspect_VKeyFlags             modifiers,
                                          bool                         isEmulated)
{
  return AIS_ViewController::UpdateMouseButtons(point, buttons, modifiers, isEmulated);
}

void ViewerInteractor::ProcessExpose()
{
  if (!m_view.IsNull())
  {
    m_view->Invalidate();
    FlushViewEvents(m_ctx, m_view, true);
  }
}

void ViewerInteractor::handleViewRedraw(const Handle(AIS_InteractiveContext)& ctx,
                                        const Handle(V3d_View)&               view)
{
  AIS_ViewController::handleViewRedraw(ctx, view);
}

void ViewerInteractor::ProcessConfigure()
{
  if (!m_view.IsNull())
  {
    m_view->MustBeResized();
    FlushViewEvents(m_ctx, m_view, true);
  }
}

void ViewerInteractor::KeyDown(Aspect_VKey key, double time, double pressure)
{
  AIS_ViewController::KeyDown(key, time, pressure);
}

void ViewerInteractor::KeyUp(Aspect_VKey key, double time)
{
  AIS_ViewController::KeyUp(key, time);
  ProcessKeyPress(key | myKeys.Modifiers());
}

void ViewerInteractor::FlushEvents()
{
  if (!m_view.IsNull())
    FlushViewEvents(m_ctx, m_view, true);
}

void ViewerInteractor::ProcessKeyPress(Aspect_VKey key)
{
  if (m_ctx.IsNull() || m_view.IsNull())
    return;

  switch (key)
  {
    case Aspect_VKey_F:
      if (m_ctx->NbSelected() > 0)
        m_ctx->FitSelected(m_view);
      else
        m_view->FitAll();
      break;

    case Aspect_VKey_S:
    case Aspect_VKey_W:
    {
      const int dm = (key == Aspect_VKey_S) ? AIS_Shaded : AIS_WireFrame;
      if (m_ctx->NbSelected() == 0)
      {
        m_ctx->SetDisplayMode(dm, false);
        m_ctx->UpdateCurrentViewer();
      }
      else
      {
        for (m_ctx->InitSelected(); m_ctx->MoreSelected(); m_ctx->NextSelected())
          m_ctx->SetDisplayMode(m_ctx->SelectedInteractive(), dm, false);
        m_ctx->UpdateCurrentViewer();
      }
      break;
    }

    case Aspect_VKey_Backspace:
      m_view->SetProj(V3d_XposYnegZpos);
      m_view->Redraw();
      break;
    case Aspect_VKey_T:
      m_view->SetProj(V3d_TypeOfOrientation_Zup_Top);
      m_view->Redraw();
      break;
    case Aspect_VKey_B:
      m_view->SetProj(V3d_TypeOfOrientation_Zup_Bottom);
      m_view->Redraw();
      break;
    case Aspect_VKey_L:
      m_view->SetProj(V3d_TypeOfOrientation_Zup_Left);
      m_view->Redraw();
      break;
    case Aspect_VKey_R:
      m_view->SetProj(V3d_TypeOfOrientation_Zup_Right);
      m_view->Redraw();
      break;
    default:
      break;
  }
}

} // namespace CellForge
