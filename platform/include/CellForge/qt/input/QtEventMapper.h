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

#include <Qt>

namespace CellForge {

struct QtEventMapper {

    static KeyCode mapKey(int qtKey)
    {
        // Printable ASCII range — Qt key values match ASCII for these.
        switch (qtKey) {
            case Qt::Key_Space:        return KeyCode::Space;
            case Qt::Key_Apostrophe:   return KeyCode::Apostrophe;
            case Qt::Key_Comma:        return KeyCode::Comma;
            case Qt::Key_Minus:        return KeyCode::Minus;
            case Qt::Key_Period:       return KeyCode::Period;
            case Qt::Key_Slash:        return KeyCode::Slash;
            case Qt::Key_0:            return KeyCode::D0;
            case Qt::Key_1:            return KeyCode::D1;
            case Qt::Key_2:            return KeyCode::D2;
            case Qt::Key_3:            return KeyCode::D3;
            case Qt::Key_4:            return KeyCode::D4;
            case Qt::Key_5:            return KeyCode::D5;
            case Qt::Key_6:            return KeyCode::D6;
            case Qt::Key_7:            return KeyCode::D7;
            case Qt::Key_8:            return KeyCode::D8;
            case Qt::Key_9:            return KeyCode::D9;
            case Qt::Key_Semicolon:    return KeyCode::Semicolon;
            case Qt::Key_Equal:        return KeyCode::Equal;
            case Qt::Key_A:            return KeyCode::A;
            case Qt::Key_B:            return KeyCode::B;
            case Qt::Key_C:            return KeyCode::C;
            case Qt::Key_D:            return KeyCode::D;
            case Qt::Key_E:            return KeyCode::E;
            case Qt::Key_F:            return KeyCode::F;
            case Qt::Key_G:            return KeyCode::G;
            case Qt::Key_H:            return KeyCode::H;
            case Qt::Key_I:            return KeyCode::I;
            case Qt::Key_J:            return KeyCode::J;
            case Qt::Key_K:            return KeyCode::K;
            case Qt::Key_L:            return KeyCode::L;
            case Qt::Key_M:            return KeyCode::M;
            case Qt::Key_N:            return KeyCode::N;
            case Qt::Key_O:            return KeyCode::O;
            case Qt::Key_P:            return KeyCode::P;
            case Qt::Key_Q:            return KeyCode::Q;
            case Qt::Key_R:            return KeyCode::R;
            case Qt::Key_S:            return KeyCode::S;
            case Qt::Key_T:            return KeyCode::T;
            case Qt::Key_U:            return KeyCode::U;
            case Qt::Key_V:            return KeyCode::V;
            case Qt::Key_W:            return KeyCode::W;
            case Qt::Key_X:            return KeyCode::X;
            case Qt::Key_Y:            return KeyCode::Y;
            case Qt::Key_Z:            return KeyCode::Z;
            case Qt::Key_BracketLeft:  return KeyCode::LeftBracket;
            case Qt::Key_Backslash:    return KeyCode::Backslash;
            case Qt::Key_BracketRight: return KeyCode::RightBracket;
            case Qt::Key_QuoteLeft:    return KeyCode::GraveAccent;

            // Function / navigation keys
            case Qt::Key_Escape:       return KeyCode::Escape;
            case Qt::Key_Return:       return KeyCode::Enter;
            case Qt::Key_Enter:        return KeyCode::KPEnter;
            case Qt::Key_Tab:          return KeyCode::Tab;
            case Qt::Key_Backspace:    return KeyCode::Backspace;
            case Qt::Key_Insert:       return KeyCode::Insert;
            case Qt::Key_Delete:       return KeyCode::Delete;
            case Qt::Key_Right:        return KeyCode::Right;
            case Qt::Key_Left:         return KeyCode::Left;
            case Qt::Key_Down:         return KeyCode::Down;
            case Qt::Key_Up:           return KeyCode::Up;
            case Qt::Key_PageUp:       return KeyCode::PageUp;
            case Qt::Key_PageDown:     return KeyCode::PageDown;
            case Qt::Key_Home:         return KeyCode::Home;
            case Qt::Key_End:          return KeyCode::End;
            case Qt::Key_CapsLock:     return KeyCode::CapsLock;
            case Qt::Key_ScrollLock:   return KeyCode::ScrollLock;
            case Qt::Key_NumLock:      return KeyCode::NumLock;
            case Qt::Key_Print:        return KeyCode::PrintScreen;
            case Qt::Key_Pause:        return KeyCode::Pause;
            case Qt::Key_F1:           return KeyCode::F1;
            case Qt::Key_F2:           return KeyCode::F2;
            case Qt::Key_F3:           return KeyCode::F3;
            case Qt::Key_F4:           return KeyCode::F4;
            case Qt::Key_F5:           return KeyCode::F5;
            case Qt::Key_F6:           return KeyCode::F6;
            case Qt::Key_F7:           return KeyCode::F7;
            case Qt::Key_F8:           return KeyCode::F8;
            case Qt::Key_F9:           return KeyCode::F9;
            case Qt::Key_F10:          return KeyCode::F10;
            case Qt::Key_F11:          return KeyCode::F11;
            case Qt::Key_F12:          return KeyCode::F12;
            case Qt::Key_F13:          return KeyCode::F13;
            case Qt::Key_F14:          return KeyCode::F14;
            case Qt::Key_F15:          return KeyCode::F15;
            case Qt::Key_F16:          return KeyCode::F16;
            case Qt::Key_F17:          return KeyCode::F17;
            case Qt::Key_F18:          return KeyCode::F18;
            case Qt::Key_F19:          return KeyCode::F19;
            case Qt::Key_F20:          return KeyCode::F20;
            case Qt::Key_F21:          return KeyCode::F21;
            case Qt::Key_F22:          return KeyCode::F22;
            case Qt::Key_F23:          return KeyCode::F23;
            case Qt::Key_F24:          return KeyCode::F24;
            case Qt::Key_F25:          return KeyCode::F25;

            // Modifiers
            case Qt::Key_Shift:        return KeyCode::LeftShift;
            case Qt::Key_Control:      return KeyCode::LeftControl;
            case Qt::Key_Alt:          return KeyCode::LeftAlt;
            case Qt::Key_Meta:         return KeyCode::LeftSuper;
            case Qt::Key_Menu:         return KeyCode::Menu;

            default:                   return KeyCode::Menu; // unknown — Menu is last valid value
        }
    }

    static MouseButton mapMouseButton(Qt::MouseButton button)
    {
        switch (button) {
            case Qt::LeftButton:   return MouseButton::Left;
            case Qt::RightButton:  return MouseButton::Right;
            case Qt::MiddleButton: return MouseButton::Middle;
            case Qt::XButton1:     return MouseButton::Button3;
            case Qt::XButton2:     return MouseButton::Button4;
            default:               return MouseButton::Button0;
        }
    }
};

} // namespace CellForge
