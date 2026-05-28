// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#include <CellForge/mfc/MfcViewerWnd.h>

#include <CellForge/Viewer.h>
#include <CellForge/ViewerInteractor.h>
#include <CellForge/Event/MouseEvents.h>
#include <CellForge/Event/ApplicationEvent.h>

#include <Aspect_ScrollDelta.hxx>
#include <Aspect_VKey.hxx>

namespace CellForge {

// ---------------------------------------------------------------------------
// Win32 → OCCT translation helpers (file-local)
// ---------------------------------------------------------------------------
namespace {

Aspect_VKeyMouse mfcFlagsToOcctButtons(UINT nFlags)
{
    Aspect_VKeyMouse r = Aspect_VKeyMouse_NONE;
    if (nFlags & MK_LBUTTON)
        r = static_cast<Aspect_VKeyMouse>(r | Aspect_VKeyMouse_LeftButton);
    if (nFlags & MK_RBUTTON)
        r = static_cast<Aspect_VKeyMouse>(r | Aspect_VKeyMouse_RightButton);
    if (nFlags & MK_MBUTTON)
        r = static_cast<Aspect_VKeyMouse>(r | Aspect_VKeyMouse_MiddleButton);
    return r;
}

Aspect_VKeyFlags mfcFlagsToOcctMods(UINT nFlags)
{
    Aspect_VKeyFlags r = Aspect_VKeyFlags_NONE;
    if (nFlags & MK_SHIFT)   r = static_cast<Aspect_VKeyFlags>(r | Aspect_VKeyFlags_SHIFT);
    if (nFlags & MK_CONTROL) r = static_cast<Aspect_VKeyFlags>(r | Aspect_VKeyFlags_CTRL);
    return r;
}

Aspect_VKeyFlags currentKeyMods()
{
    Aspect_VKeyFlags r = Aspect_VKeyFlags_NONE;
    if (::GetKeyState(VK_SHIFT)   & 0x8000) r = static_cast<Aspect_VKeyFlags>(r | Aspect_VKeyFlags_SHIFT);
    if (::GetKeyState(VK_CONTROL) & 0x8000) r = static_cast<Aspect_VKeyFlags>(r | Aspect_VKeyFlags_CTRL);
    if (::GetKeyState(VK_MENU)    & 0x8000) r = static_cast<Aspect_VKeyFlags>(r | Aspect_VKeyFlags_ALT);
    return r;
}

Aspect_VKey vkToOcct(UINT vk)
{
    switch (vk)
    {
        case 'A': return Aspect_VKey_A;  case 'B': return Aspect_VKey_B;
        case 'C': return Aspect_VKey_C;  case 'D': return Aspect_VKey_D;
        case 'E': return Aspect_VKey_E;  case 'F': return Aspect_VKey_F;
        case 'G': return Aspect_VKey_G;  case 'H': return Aspect_VKey_H;
        case 'I': return Aspect_VKey_I;  case 'J': return Aspect_VKey_J;
        case 'K': return Aspect_VKey_K;  case 'L': return Aspect_VKey_L;
        case 'M': return Aspect_VKey_M;  case 'N': return Aspect_VKey_N;
        case 'O': return Aspect_VKey_O;  case 'P': return Aspect_VKey_P;
        case 'Q': return Aspect_VKey_Q;  case 'R': return Aspect_VKey_R;
        case 'S': return Aspect_VKey_S;  case 'T': return Aspect_VKey_T;
        case 'U': return Aspect_VKey_U;  case 'V': return Aspect_VKey_V;
        case 'W': return Aspect_VKey_W;  case 'X': return Aspect_VKey_X;
        case 'Y': return Aspect_VKey_Y;  case 'Z': return Aspect_VKey_Z;
        case '0': return Aspect_VKey_0;  case '1': return Aspect_VKey_1;
        case '2': return Aspect_VKey_2;  case '3': return Aspect_VKey_3;
        case '4': return Aspect_VKey_4;  case '5': return Aspect_VKey_5;
        case '6': return Aspect_VKey_6;  case '7': return Aspect_VKey_7;
        case '8': return Aspect_VKey_8;  case '9': return Aspect_VKey_9;
        case VK_F1:      return Aspect_VKey_F1;  case VK_F2:  return Aspect_VKey_F2;
        case VK_F3:      return Aspect_VKey_F3;  case VK_F4:  return Aspect_VKey_F4;
        case VK_F5:      return Aspect_VKey_F5;  case VK_F6:  return Aspect_VKey_F6;
        case VK_F7:      return Aspect_VKey_F7;  case VK_F8:  return Aspect_VKey_F8;
        case VK_F9:      return Aspect_VKey_F9;  case VK_F10: return Aspect_VKey_F10;
        case VK_F11:     return Aspect_VKey_F11; case VK_F12: return Aspect_VKey_F12;
        case VK_UP:      return Aspect_VKey_Up;
        case VK_DOWN:    return Aspect_VKey_Down;
        case VK_LEFT:    return Aspect_VKey_Left;
        case VK_RIGHT:   return Aspect_VKey_Right;
        case VK_PRIOR:   return Aspect_VKey_PageUp;
        case VK_NEXT:    return Aspect_VKey_PageDown;
        case VK_HOME:    return Aspect_VKey_Home;
        case VK_END:     return Aspect_VKey_End;
        case VK_ESCAPE:  return Aspect_VKey_Escape;
        case VK_BACK:    return Aspect_VKey_Backspace;
        case VK_RETURN:  return Aspect_VKey_Enter;
        case VK_DELETE:  return Aspect_VKey_Delete;
        case VK_SPACE:   return Aspect_VKey_Space;
        case VK_TAB:     return Aspect_VKey_Tab;
        case VK_SHIFT:   return Aspect_VKey_Shift;
        case VK_CONTROL: return Aspect_VKey_Control;
        case VK_MENU:    return Aspect_VKey_Alt;
        case VK_ADD:     return Aspect_VKey_Plus;
        case VK_SUBTRACT: return Aspect_VKey_Minus;
        default:         return Aspect_VKey_UNKNOWN;
    }
}

} // anonymous namespace

// ---------------------------------------------------------------------------
// MfcViewerWnd
// ---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(MfcViewerWnd, CWnd)
    ON_WM_CREATE()
    ON_WM_PAINT()
    ON_WM_SIZE()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDOWN()
    ON_WM_RBUTTONDOWN()
    ON_WM_MBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_RBUTTONUP()
    ON_WM_MBUTTONUP()
    ON_WM_MOUSEWHEEL()
    ON_WM_KEYDOWN()
    ON_WM_KEYUP()
END_MESSAGE_MAP()

MfcViewerWnd::MfcViewerWnd() = default;
MfcViewerWnd::~MfcViewerWnd() = default;

bool MfcViewerWnd::create(CWnd* pParent, const RECT& rect)
{
    // CS_OWNDC: dedicated device context required by the OpenGL driver.
    LPCTSTR cls = AfxRegisterWndClass(
        CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
        ::LoadCursor(nullptr, IDC_ARROW),
        static_cast<HBRUSH>(::GetStockObject(BLACK_BRUSH)),
        nullptr);

    return CWnd::Create(cls, _T("CFViewerWnd"),
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        rect, pParent, 2u) == TRUE;
}

void MfcViewerWnd::setEventCallback(EventCallbackFn cb)
{
    m_callback = std::move(cb);
}

void MfcViewerWnd::resize(int cx, int cy)
{
    if (m_hWnd)
        ::MoveWindow(m_hWnd, 0, 0, cx, cy, TRUE);
}

// ── IViewerWidget shape API ─────────────────────────────────────────────────

void MfcViewerWnd::displayShapes(const std::vector<TopoDS_Shape>& shapes)
{
    if (m_viewer) m_viewer->displayShapes(shapes);
}

void MfcViewerWnd::clearShapes()
{
    if (m_viewer) m_viewer->clearShapes();
}

void MfcViewerWnd::fitAll()
{
    if (m_viewer) m_viewer->fitAll();
}

void MfcViewerWnd::setBackgroundColor(const Quantity_Color& color)
{
    if (m_viewer) m_viewer->setBackgroundColor(color);
}

void MfcViewerWnd::setDisplayMode(AIS_DisplayMode mode)
{
    if (m_viewer) m_viewer->setDisplayMode(mode);
}

// ── Win32 message handlers ──────────────────────────────────────────────────

int MfcViewerWnd::OnCreate(LPCREATESTRUCT lpcs)
{
    if (CWnd::OnCreate(lpcs) == -1)
        return -1;
    m_viewer = std::make_unique<Viewer>((Aspect_Handle) m_hWnd);
    return 0;
}

void MfcViewerWnd::OnPaint()
{
    CPaintDC dc(this);
    m_viewer->redrawView();
}

void MfcViewerWnd::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);
    if (m_viewer)
        m_viewer->interactor()->ProcessConfigure();

    if (m_callback)
    {
        WindowResizeEvent e(static_cast<uint32_t>(cx), static_cast<uint32_t>(cy));
        m_callback(e);
    }
}

void MfcViewerWnd::OnMouseMove(UINT nFlags, CPoint pt)
{
    if (!m_viewer) return;
    m_viewer->interactor()->UpdateMousePosition(
        NCollection_Vec2<int>(pt.x, pt.y),
        mfcFlagsToOcctButtons(nFlags),
        mfcFlagsToOcctMods(nFlags), false);
    m_viewer->interactor()->FlushEvents();
}

void MfcViewerWnd::handleMouseDown(UINT nFlags, CPoint pt)
{
    SetCapture();
    SetFocus();
    m_viewer->interactor()->UpdateMouseButtons(
        NCollection_Vec2<int>(pt.x, pt.y),
        mfcFlagsToOcctButtons(nFlags),
        mfcFlagsToOcctMods(nFlags), false);
    m_viewer->interactor()->FlushEvents();
}

void MfcViewerWnd::handleMouseUp(UINT nFlags, CPoint pt)
{
    m_viewer->interactor()->UpdateMouseButtons(
        NCollection_Vec2<int>(pt.x, pt.y),
        mfcFlagsToOcctButtons(nFlags),
        mfcFlagsToOcctMods(nFlags), false);
    m_viewer->interactor()->FlushEvents();

    if (!(nFlags & (MK_LBUTTON | MK_RBUTTON | MK_MBUTTON)))
        ReleaseCapture();
}

void MfcViewerWnd::OnLButtonDown(UINT nFlags, CPoint pt) { if (m_viewer) handleMouseDown(nFlags | MK_LBUTTON, pt); }
void MfcViewerWnd::OnRButtonDown(UINT nFlags, CPoint pt) { if (m_viewer) handleMouseDown(nFlags | MK_RBUTTON, pt); }
void MfcViewerWnd::OnMButtonDown(UINT nFlags, CPoint pt) { if (m_viewer) handleMouseDown(nFlags | MK_MBUTTON, pt); }

void MfcViewerWnd::OnLButtonUp(UINT nFlags, CPoint pt)
{
    if (!m_viewer) return;
    handleMouseUp(nFlags, pt);
    if (m_callback)
    {
        MouseButtonReleasedEvent e(MouseButton::Left,
            static_cast<float>(pt.x), static_cast<float>(pt.y));
        m_callback(e);
    }
}
void MfcViewerWnd::OnRButtonUp(UINT nFlags, CPoint pt) { if (m_viewer) handleMouseUp(nFlags, pt); }
void MfcViewerWnd::OnMButtonUp(UINT nFlags, CPoint pt) { if (m_viewer) handleMouseUp(nFlags, pt); }

BOOL MfcViewerWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    if (!m_viewer) return FALSE;
    ScreenToClient(&pt);
    m_viewer->interactor()->UpdateMouseScroll(
        Aspect_ScrollDelta(NCollection_Vec2<int>(pt.x, pt.y), zDelta / 120.0));
    m_viewer->interactor()->FlushEvents();
    return TRUE;
}

void MfcViewerWnd::OnKeyDown(UINT nChar, UINT /*nRepCnt*/, UINT /*nFlags*/)
{
    if (!m_viewer) return;
    const Aspect_VKey vk = vkToOcct(nChar);
    if (vk != Aspect_VKey_UNKNOWN)
        m_viewer->interactor()->KeyDown(vk, ::GetTickCount64() / 1000.0);
    m_viewer->interactor()->FlushEvents();
}

void MfcViewerWnd::OnKeyUp(UINT nChar, UINT /*nRepCnt*/, UINT /*nFlags*/)
{
    if (!m_viewer) return;
    const Aspect_VKey vk = vkToOcct(nChar);
    if (vk != Aspect_VKey_UNKNOWN)
        m_viewer->interactor()->KeyUp(vk, ::GetTickCount64() / 1000.0);
    m_viewer->interactor()->FlushEvents();
}

} // namespace CellForge
