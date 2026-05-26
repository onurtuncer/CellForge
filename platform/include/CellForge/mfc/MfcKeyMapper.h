// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#include <CellForge/Input/KeyCodes.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

namespace CellForge {

struct MfcKeyMapper {

    // Maps a Windows virtual-key code (VK_*) to a CellForge KeyCode.
    // Letters A-Z and digits 0-9 share values with CellForge/GLFW, so they
    // fall through to the default cast. All other keys need explicit mapping.
    static KeyCode mapKey(UINT vk)
    {
        switch (vk) {
            // ── Printable / ASCII-range ───────────────────────────────────
            case VK_SPACE:      return KeyCode::Space;
            case VK_OEM_7:      return KeyCode::Apostrophe;   // ' "
            case VK_OEM_COMMA:  return KeyCode::Comma;
            case VK_OEM_MINUS:  return KeyCode::Minus;
            case VK_OEM_PERIOD: return KeyCode::Period;
            case VK_OEM_2:      return KeyCode::Slash;        // / ?
            case VK_OEM_1:      return KeyCode::Semicolon;    // ; :
            case VK_OEM_PLUS:   return KeyCode::Equal;        // = +
            case VK_OEM_4:      return KeyCode::LeftBracket;  // [ {
            case VK_OEM_5:      return KeyCode::Backslash;    // \ |
            case VK_OEM_6:      return KeyCode::RightBracket; // ] }
            case VK_OEM_3:      return KeyCode::GraveAccent;  // ` ~

            // ── Navigation ───────────────────────────────────────────────
            case VK_ESCAPE:     return KeyCode::Escape;
            case VK_RETURN:     return KeyCode::Enter;
            case VK_TAB:        return KeyCode::Tab;
            case VK_BACK:       return KeyCode::Backspace;
            case VK_INSERT:     return KeyCode::Insert;
            case VK_DELETE:     return KeyCode::Delete;
            case VK_RIGHT:      return KeyCode::Right;
            case VK_LEFT:       return KeyCode::Left;
            case VK_DOWN:       return KeyCode::Down;
            case VK_UP:         return KeyCode::Up;
            case VK_PRIOR:      return KeyCode::PageUp;
            case VK_NEXT:       return KeyCode::PageDown;
            case VK_HOME:       return KeyCode::Home;
            case VK_END:        return KeyCode::End;

            // ── Lock keys ────────────────────────────────────────────────
            case VK_CAPITAL:    return KeyCode::CapsLock;
            case VK_SCROLL:     return KeyCode::ScrollLock;
            case VK_NUMLOCK:    return KeyCode::NumLock;
            case VK_SNAPSHOT:   return KeyCode::PrintScreen;
            case VK_PAUSE:      return KeyCode::Pause;

            // ── Function keys ─────────────────────────────────────────────
            case VK_F1:  return KeyCode::F1;   case VK_F2:  return KeyCode::F2;
            case VK_F3:  return KeyCode::F3;   case VK_F4:  return KeyCode::F4;
            case VK_F5:  return KeyCode::F5;   case VK_F6:  return KeyCode::F6;
            case VK_F7:  return KeyCode::F7;   case VK_F8:  return KeyCode::F8;
            case VK_F9:  return KeyCode::F9;   case VK_F10: return KeyCode::F10;
            case VK_F11: return KeyCode::F11;  case VK_F12: return KeyCode::F12;
            case VK_F13: return KeyCode::F13;  case VK_F14: return KeyCode::F14;
            case VK_F15: return KeyCode::F15;  case VK_F16: return KeyCode::F16;
            case VK_F17: return KeyCode::F17;  case VK_F18: return KeyCode::F18;
            case VK_F19: return KeyCode::F19;  case VK_F20: return KeyCode::F20;
            case VK_F21: return KeyCode::F21;  case VK_F22: return KeyCode::F22;
            case VK_F23: return KeyCode::F23;  case VK_F24: return KeyCode::F24;

            // ── Numpad ───────────────────────────────────────────────────
            case VK_NUMPAD0: return KeyCode::KP0;
            case VK_NUMPAD1: return KeyCode::KP1;
            case VK_NUMPAD2: return KeyCode::KP2;
            case VK_NUMPAD3: return KeyCode::KP3;
            case VK_NUMPAD4: return KeyCode::KP4;
            case VK_NUMPAD5: return KeyCode::KP5;
            case VK_NUMPAD6: return KeyCode::KP6;
            case VK_NUMPAD7: return KeyCode::KP7;
            case VK_NUMPAD8: return KeyCode::KP8;
            case VK_NUMPAD9: return KeyCode::KP9;
            case VK_DECIMAL:  return KeyCode::KPDecimal;
            case VK_DIVIDE:   return KeyCode::KPDivide;
            case VK_MULTIPLY: return KeyCode::KPMultiply;
            case VK_SUBTRACT: return KeyCode::KPSubtract;
            case VK_ADD:      return KeyCode::KPAdd;

            // ── Modifiers ────────────────────────────────────────────────
            case VK_LSHIFT:   return KeyCode::LeftShift;
            case VK_RSHIFT:   return KeyCode::RightShift;
            case VK_LCONTROL: return KeyCode::LeftControl;
            case VK_RCONTROL: return KeyCode::RightControl;
            case VK_LMENU:    return KeyCode::LeftAlt;
            case VK_RMENU:    return KeyCode::RightAlt;
            case VK_LWIN:     return KeyCode::LeftSuper;
            case VK_RWIN:     return KeyCode::RightSuper;
            case VK_APPS:     return KeyCode::Menu;

            // ── Letters (VK_A-Z == 65-90 == KeyCode::A-Z) and
            //    Digits (VK_0-9 == 48-57 == KeyCode::D0-D9): direct cast. ──
            default:
                if ((vk >= 0x30 && vk <= 0x39) || (vk >= 0x41 && vk <= 0x5A))
                    return static_cast<KeyCode>(vk);
                return KeyCode::Menu; // unknown — last valid enum value
        }
    }

    static MouseButton mapMouseButton(UINT msg, WPARAM wParam)
    {
        switch (msg) {
            case WM_LBUTTONDOWN: case WM_LBUTTONUP: return MouseButton::Left;
            case WM_RBUTTONDOWN: case WM_RBUTTONUP: return MouseButton::Right;
            case WM_MBUTTONDOWN: case WM_MBUTTONUP: return MouseButton::Middle;
            case WM_XBUTTONDOWN: case WM_XBUTTONUP:
                return (HIWORD(wParam) == XBUTTON1) ? MouseButton::Button3
                                                    : MouseButton::Button4;
            default: return MouseButton::Left;
        }
    }
};

} // namespace CellForge
