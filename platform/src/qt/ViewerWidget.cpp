// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#include <CellForge/qt/widgets/ViewerWidget.h>
#include <CellForge/qt/input/QtEventMapper.h>

#include <CellForge/Event/MouseEvents.h>
#include <CellForge/Event/ApplicationEvent.h>
#include <CellForge/Viewer.h>
#include <CellForge/ViewerInteractor.h>

#include <QKeyEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QWheelEvent>

#include <Aspect_Handle.hxx>
#include <Aspect_ScrollDelta.hxx>
#include <Aspect_VKey.hxx>

namespace CellForge {

// ---------------------------------------------------------------------------
// Qt → OCCT translation helpers (file-local)
// ---------------------------------------------------------------------------
namespace {

Aspect_VKeyMouse qtButtonsToOcct(Qt::MouseButtons buttons)
{
    Aspect_VKeyMouse r = Aspect_VKeyMouse_NONE;
    if (buttons & Qt::LeftButton)
        r = static_cast<Aspect_VKeyMouse>(r | Aspect_VKeyMouse_LeftButton);
    if (buttons & Qt::RightButton)
        r = static_cast<Aspect_VKeyMouse>(r | Aspect_VKeyMouse_RightButton);
    if (buttons & Qt::MiddleButton)
        r = static_cast<Aspect_VKeyMouse>(r | Aspect_VKeyMouse_MiddleButton);
    return r;
}

Aspect_VKeyFlags qtModsToOcct(Qt::KeyboardModifiers mods)
{
    Aspect_VKeyFlags r = Aspect_VKeyFlags_NONE;
    if (mods & Qt::ShiftModifier)
        r = static_cast<Aspect_VKeyFlags>(r | Aspect_VKeyFlags_SHIFT);
    if (mods & Qt::ControlModifier)
        r = static_cast<Aspect_VKeyFlags>(r | Aspect_VKeyFlags_CTRL);
    if (mods & Qt::AltModifier)
        r = static_cast<Aspect_VKeyFlags>(r | Aspect_VKeyFlags_ALT);
    return r;
}

Aspect_VKey qtKeyToOcct(int qtKey)
{
    switch (qtKey)
    {
        case Qt::Key_A: return Aspect_VKey_A;  case Qt::Key_B: return Aspect_VKey_B;
        case Qt::Key_C: return Aspect_VKey_C;  case Qt::Key_D: return Aspect_VKey_D;
        case Qt::Key_E: return Aspect_VKey_E;  case Qt::Key_F: return Aspect_VKey_F;
        case Qt::Key_G: return Aspect_VKey_G;  case Qt::Key_H: return Aspect_VKey_H;
        case Qt::Key_I: return Aspect_VKey_I;  case Qt::Key_J: return Aspect_VKey_J;
        case Qt::Key_K: return Aspect_VKey_K;  case Qt::Key_L: return Aspect_VKey_L;
        case Qt::Key_M: return Aspect_VKey_M;  case Qt::Key_N: return Aspect_VKey_N;
        case Qt::Key_O: return Aspect_VKey_O;  case Qt::Key_P: return Aspect_VKey_P;
        case Qt::Key_Q: return Aspect_VKey_Q;  case Qt::Key_R: return Aspect_VKey_R;
        case Qt::Key_S: return Aspect_VKey_S;  case Qt::Key_T: return Aspect_VKey_T;
        case Qt::Key_U: return Aspect_VKey_U;  case Qt::Key_V: return Aspect_VKey_V;
        case Qt::Key_W: return Aspect_VKey_W;  case Qt::Key_X: return Aspect_VKey_X;
        case Qt::Key_Y: return Aspect_VKey_Y;  case Qt::Key_Z: return Aspect_VKey_Z;
        case Qt::Key_0: return Aspect_VKey_0;  case Qt::Key_1: return Aspect_VKey_1;
        case Qt::Key_2: return Aspect_VKey_2;  case Qt::Key_3: return Aspect_VKey_3;
        case Qt::Key_4: return Aspect_VKey_4;  case Qt::Key_5: return Aspect_VKey_5;
        case Qt::Key_6: return Aspect_VKey_6;  case Qt::Key_7: return Aspect_VKey_7;
        case Qt::Key_8: return Aspect_VKey_8;  case Qt::Key_9: return Aspect_VKey_9;
        case Qt::Key_F1:  return Aspect_VKey_F1;  case Qt::Key_F2:  return Aspect_VKey_F2;
        case Qt::Key_F3:  return Aspect_VKey_F3;  case Qt::Key_F4:  return Aspect_VKey_F4;
        case Qt::Key_F5:  return Aspect_VKey_F5;  case Qt::Key_F6:  return Aspect_VKey_F6;
        case Qt::Key_F7:  return Aspect_VKey_F7;  case Qt::Key_F8:  return Aspect_VKey_F8;
        case Qt::Key_F9:  return Aspect_VKey_F9;  case Qt::Key_F10: return Aspect_VKey_F10;
        case Qt::Key_F11: return Aspect_VKey_F11; case Qt::Key_F12: return Aspect_VKey_F12;
        case Qt::Key_Up:        return Aspect_VKey_Up;
        case Qt::Key_Down:      return Aspect_VKey_Down;
        case Qt::Key_Left:      return Aspect_VKey_Left;
        case Qt::Key_Right:     return Aspect_VKey_Right;
        case Qt::Key_PageUp:    return Aspect_VKey_PageUp;
        case Qt::Key_PageDown:  return Aspect_VKey_PageDown;
        case Qt::Key_Home:      return Aspect_VKey_Home;
        case Qt::Key_End:       return Aspect_VKey_End;
        case Qt::Key_Escape:    return Aspect_VKey_Escape;
        case Qt::Key_Backspace: return Aspect_VKey_Backspace;
        case Qt::Key_Return:
        case Qt::Key_Enter:     return Aspect_VKey_Enter;
        case Qt::Key_Delete:    return Aspect_VKey_Delete;
        case Qt::Key_Space:     return Aspect_VKey_Space;
        case Qt::Key_Tab:       return Aspect_VKey_Tab;
        case Qt::Key_Shift:     return Aspect_VKey_Shift;
        case Qt::Key_Control:   return Aspect_VKey_Control;
        case Qt::Key_Alt:       return Aspect_VKey_Alt;
        case Qt::Key_Plus:      return Aspect_VKey_Plus;
        case Qt::Key_Minus:     return Aspect_VKey_Minus;
        case Qt::Key_Equal:     return Aspect_VKey_Equal;
        default:                return Aspect_VKey_UNKNOWN;
    }
}

} // anonymous namespace

// ---------------------------------------------------------------------------
// ViewerWidget
// ---------------------------------------------------------------------------

ViewerWidget::ViewerWidget(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    m_timer.start();
}

ViewerWidget::~ViewerWidget()
{
    delete m_viewer;
}

void ViewerWidget::resize(int cx, int cy)
{
    QWidget::resize(cx, cy);
}

void ViewerWidget::setEventCallback(EventCallbackFn cb)
{
    m_callback = std::move(cb);
}

void ViewerWidget::ensureViewer()
{
    if (!m_isInitialized)
    {
        m_viewer        = new Viewer((Aspect_Handle) winId());
        m_isInitialized = true;
    }
}

// ── IViewerWidget shape API ─────────────────────────────────────────────────

void ViewerWidget::displayShapes(const std::vector<TopoDS_Shape>& shapes)
{
    ensureViewer();
    m_viewer->displayShapes(shapes);
}

void ViewerWidget::clearShapes()
{
    if (m_viewer) m_viewer->clearShapes();
}

void ViewerWidget::fitAll()
{
    if (m_viewer) m_viewer->fitAll();
}

void ViewerWidget::setBackgroundColor(const Quantity_Color& color)
{
    if (m_viewer) m_viewer->setBackgroundColor(color);
}

void ViewerWidget::setDisplayMode(AIS_DisplayMode mode)
{
    if (m_viewer) m_viewer->setDisplayMode(mode);
}

// ── Qt event overrides ──────────────────────────────────────────────────────

void ViewerWidget::paintEvent(QPaintEvent* /*event*/)
{
    ensureViewer();
    m_viewer->redrawView();
}

void ViewerWidget::resizeEvent(QResizeEvent* event)
{
    ensureViewer();
    // ProcessConfigure calls MustBeResized + FlushViewEvents in one shot.
    m_viewer->interactor()->ProcessConfigure();

    if (m_callback)
    {
        WindowResizeEvent e(static_cast<uint32_t>(event->size().width()),
                            static_cast<uint32_t>(event->size().height()));
        m_callback(e);
    }
    QWidget::resizeEvent(event);
}

void ViewerWidget::mousePressEvent(QMouseEvent* event)
{
    ensureViewer();
    setFocus();

    const NCollection_Vec2<int> pos(event->pos().x(), event->pos().y());
    m_viewer->interactor()->UpdateMouseButtons(
        pos, qtButtonsToOcct(event->buttons()), qtModsToOcct(event->modifiers()), false);
    m_viewer->interactor()->FlushEvents();

    if (m_callback)
    {
        MouseButtonPressedEvent e(QtEventMapper::mapMouseButton(event->button()),
                                  static_cast<float>(event->pos().x()),
                                  static_cast<float>(event->pos().y()));
        m_callback(e);
    }
}

void ViewerWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_isInitialized) return;

    const NCollection_Vec2<int> pos(event->pos().x(), event->pos().y());
    m_viewer->interactor()->UpdateMousePosition(
        pos, qtButtonsToOcct(event->buttons()), qtModsToOcct(event->modifiers()), false);
    m_viewer->interactor()->FlushEvents();
}

void ViewerWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (!m_isInitialized) return;

    const NCollection_Vec2<int> pos(event->pos().x(), event->pos().y());
    m_viewer->interactor()->UpdateMouseButtons(
        pos, qtButtonsToOcct(event->buttons()), qtModsToOcct(event->modifiers()), false);
    m_viewer->interactor()->FlushEvents();

    if (m_callback)
    {
        MouseButtonReleasedEvent e(QtEventMapper::mapMouseButton(event->button()),
                                   static_cast<float>(event->pos().x()),
                                   static_cast<float>(event->pos().y()));
        m_callback(e);
    }
}

void ViewerWidget::wheelEvent(QWheelEvent* event)
{
    ensureViewer();

    const int delta = event->angleDelta().y();
    if (delta == 0) return;

    const NCollection_Vec2<int> pos(static_cast<int>(event->position().x()),
                                    static_cast<int>(event->position().y()));
    m_viewer->interactor()->UpdateMouseScroll(Aspect_ScrollDelta(pos, delta / 120.0));
    m_viewer->interactor()->FlushEvents();
    event->accept();
}

void ViewerWidget::keyPressEvent(QKeyEvent* event)
{
    if (!m_isInitialized) { QWidget::keyPressEvent(event); return; }

    const Aspect_VKey vk = qtKeyToOcct(event->key());
    if (vk != Aspect_VKey_UNKNOWN)
        m_viewer->interactor()->KeyDown(vk, m_timer.elapsed() / 1000.0);
    m_viewer->interactor()->FlushEvents();
    QWidget::keyPressEvent(event);
}

void ViewerWidget::keyReleaseEvent(QKeyEvent* event)
{
    if (!m_isInitialized) { QWidget::keyReleaseEvent(event); return; }

    const Aspect_VKey vk = qtKeyToOcct(event->key());
    if (vk != Aspect_VKey_UNKNOWN)
        m_viewer->interactor()->KeyUp(vk, m_timer.elapsed() / 1000.0);
    m_viewer->interactor()->FlushEvents();
    QWidget::keyReleaseEvent(event);
}

} // namespace CellForge
