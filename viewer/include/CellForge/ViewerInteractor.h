// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#include <AIS_ViewController.hxx>
#include <NCollection_Array1.hxx>
#include <TCollection_AsciiString.hxx>
#include <gp_Pnt2d.hxx>

class AIS_InteractiveContext;
class V3d_View;

namespace CellForge {

class ViewerInteractor : public Standard_Transient, public AIS_ViewController
{
public:
  DEFINE_STANDARD_RTTI_INLINE(ViewerInteractor, Standard_Transient)

  ViewerInteractor(const Handle(V3d_View)&               view,
                   const Handle(AIS_InteractiveContext)& ctx);
  virtual ~ViewerInteractor();

  const Handle(AIS_InteractiveContext)& GetContext() const { return m_ctx; }

  virtual bool UpdateMouseButtons(const NCollection_Vec2<int>& thePoint,
                                  Aspect_VKeyMouse             theButtons,
                                  Aspect_VKeyFlags             theModifiers,
                                  bool                         theIsEmulated) override;

  virtual void KeyDown(Aspect_VKey theKey,
                       double      theTime,
                       double      thePressure = 1.0) override;

  virtual void KeyUp(Aspect_VKey theKey,
                     double      theTime) override;

  virtual void ProcessExpose();

  virtual void handleViewRedraw(const Handle(AIS_InteractiveContext)& theCtx,
                                const Handle(V3d_View)&               theView) override;

  virtual void ProcessConfigure();

  void ProcessKeyPress(Aspect_VKey theKey);

  // Flush accumulated view events and redraw. Call after forwarding each
  // Qt input event (mouse move/press/wheel/key) from the platform layer.
  void FlushEvents();

private:
  Handle(V3d_View)               m_view;
  Handle(AIS_InteractiveContext) m_ctx;
};

} // namespace CellForge
