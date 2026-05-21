#pragma once

#include <AIS_ViewController.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TCollection_AsciiString.hxx>

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

  virtual bool UpdateMouseButtons(const Graphic3d_Vec2i& thePoint,
                                  Aspect_VKeyMouse       theButtons,
                                  Aspect_VKeyFlags       theModifiers,
                                  bool                   theIsEmulated) Standard_OVERRIDE;

  virtual void KeyDown(Aspect_VKey theKey,
                       double      theTime,
                       double      thePressure = 1.0) Standard_OVERRIDE;

  virtual void KeyUp(Aspect_VKey theKey,
                     double      theTime) Standard_OVERRIDE;

  virtual void ProcessExpose();

  virtual void handleViewRedraw(const Handle(AIS_InteractiveContext)& theCtx,
                                const Handle(V3d_View)&               theView) Standard_OVERRIDE;

  virtual void ProcessConfigure();

  void ProcessKeyPress(Aspect_VKey theKey);

private:
  Handle(V3d_View)               m_view;
  Handle(AIS_InteractiveContext) m_ctx;
};

} // namespace CellForge
