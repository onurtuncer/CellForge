// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#include <ostream>
#include <cstdint>

namespace CellForge{

    
enum class KeyCode : uint16_t {

None = 0,
Space = 32,
Apostrophe = 39, /* ' */
Comma = 44, /* , */
Minus = 45, /* - */
Period = 46, /* . */
Slash = 47, /* / */

D0 = 48, /* 0 */
D1 = 49, /* 1 */
D2 = 50, /* 2 */
D3 = 51, /* 3 */
D4 = 52, /* 4 */
D5 = 53, /* 5 */
D6 = 54, /* 6 */
D7 = 55, /* 7 */
D8 = 56, /* 8 */
D9 = 57, /* 9 */

Semicolon = 59, /* ; */
Equal = 61, /* = */

		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,

		LeftBracket, /* [ */
		Backslash ,  /* \ */
		RightBracket,  /* ] */
		GraveAccent = 96,  /* ` */

		World1 = 161, /* non-US #1 */
		World2 = 162, /* non-US #2 */

		/* Function keys */
		Escape = 256,
		Enter = 257,
		Tab = 258,
		Backspace = 259,
		Insert = 260,
		Delete = 261,
		Right = 262,
		Left = 263,
		Down = 264,
		Up = 265,
		PageUp = 266,
		PageDown = 267,
		Home = 268,
		End = 269,
		CapsLock = 280,
		ScrollLock = 281,
		NumLock = 282,
		PrintScreen = 283,
		Pause = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,

		/* Keypad */
		KP0 = 320,
		KP1 = 321,
		KP2 = 322,
		KP3 = 323,
		KP4 = 324,
		KP5 = 325,
		KP6 = 326,
		KP7 = 327,
		KP8 = 328,
		KP9 = 329,
		KPDecimal = 330,
		KPDivide = 331,
		KPMultiply = 332,
		KPSubtract = 333,
		KPAdd = 334,
		KPEnter = 335,
		KPEqual = 336,

		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuper = 343,
		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,
		Menu = 348
};

enum class KeyState
	{
		None = -1,
		Pressed,
		Held,
		Released
	};

	enum class CursorMode
	{
		Normal = 0,
		Hidden = 1,
		Locked = 2
	};

    	typedef enum class MouseButton : uint16_t
	{
		Button0 = 0,
		Button1 = 1,
		Button2 = 2,
		Button3 = 3,
		Button4 = 4,
		Button5 = 5,
		Left = Button0,
		Right = Button1,
		Middle = Button2
	} Button;

    inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode)
	{
		os << static_cast<int32_t>(keyCode);
		return os;
	}

	inline std::ostream& operator<<(std::ostream& os, MouseButton button)
	{
		os << static_cast<int32_t>(button);
		return os;
	}
} // namespace CellForge



       
#define CF_KEY_SPACE           ::CellForge::Key::Space
#define CF_KEY_APOSTROPHE      ::CellForge::Key::Apostrophe    /* ' */
#define CF_KEY_COMMA           ::CellForge::Key::Comma         /* , */
#define CF_KEY_MINUS           ::CellForge::Key::Minus         /* - */
#define CF_KEY_PERIOD          ::CellForge::Key::Period        /* . */
#define CF_KEY_SLASH           ::CellForge::Key::Slash         /* / */
#define CF_KEY_0               ::CellForge::Key::D0
#define CF_KEY_1               ::CellForge::Key::D1
#define CF_KEY_2               ::CellForge::Key::D2
#define CF_KEY_3               ::CellForge::Key::D3
#define CF_KEY_4               ::CellForge::Key::D4
#define CF_KEY_5               ::CellForge::Key::D5
#define CF_KEY_6               ::CellForge::Key::D6
#define CF_KEY_7               ::CellForge::Key::D7
#define CF_KEY_8               ::CellForge::Key::D8
#define CF_KEY_9               ::CellForge::Key::D9
#define CF_KEY_SEMICOLON       ::CellForge::Key::Semicolon     /* ; */
#define CF_KEY_EQUAL           ::CellForge::Key::Equal         /* = */
#define CF_KEY_A               ::CellForge::Key::A
#define CF_KEY_B               ::CellForge::Key::B
#define CF_KEY_C               ::CellForge::Key::C
#define CF_KEY_D               ::CellForge::Key::D
#define CF_KEY_E               ::CellForge::Key::E
#define CF_KEY_F               ::CellForge::Key::F
#define CF_KEY_G               ::CellForge::Key::G
#define CF_KEY_H               ::CellForge::Key::H
#define CF_KEY_I               ::CellForge::Key::I
#define CF_KEY_J               ::CellForge::Key::J
#define CF_KEY_K               ::CellForge::Key::K
#define CF_KEY_L               ::CellForge::Key::L
#define CF_KEY_M               ::CellForge::Key::M
#define CF_KEY_N               ::CellForge::Key::N
#define CF_KEY_O               ::CellForge::Key::O
#define CF_KEY_P               ::CellForge::Key::P
#define CF_KEY_Q               ::CellForge::Key::Q
#define CF_KEY_R               ::CellForge::Key::R
#define CF_KEY_S               ::CellForge::Key::S
#define CF_KEY_T               ::CellForge::Key::T
#define CF_KEY_U               ::CellForge::Key::U
#define CF_KEY_V               ::CellForge::Key::V
#define CF_KEY_W               ::CellForge::Key::W
#define CF_KEY_X               ::CellForge::Key::X
#define CF_KEY_Y               ::CellForge::Key::Y
#define CF_KEY_Z               ::CellForge::Key::Z
#define CF_KEY_LEFT_BRACKET    ::CellForge::Key::LeftBracket   /* [ */
#define CF_KEY_BACKSLASH       ::CellForge::Key::Backslash     /* \ */
#define CF_KEY_RIGHT_BRACKET   ::CellForge::Key::RightBracket  /* ] */
#define CF_KEY_GRAVE_ACCENT    ::CellForge::Key::GraveAccent   /* ` */
#define CF_KEY_WORLD_1         ::CellForge::Key::World1        /* non-US #1 */
#define CF_KEY_WORLD_2         ::CellForge::Key::World2        /* non-US #2 */

/* Function keys */
#define CF_KEY_ESCAPE          ::CellForge::Key::Escape
#define CF_KEY_ENTER           ::CellForge::Key::Enter
#define CF_KEY_TAB             ::CellForge::Key::Tab
#define CF_KEY_BACKSPACE       ::CellForge::Key::Backspace
#define CF_KEY_INSERT          ::CellForge::Key::Insert
#define CF_KEY_DELETE          ::CellForge::Key::Delete
#define CF_KEY_RIGHT           ::CellForge::Key::Right
#define CF_KEY_LEFT            ::CellForge::Key::Left
#define CF_KEY_DOWN            ::CellForge::Key::Down
#define CF_KEY_UP              ::CellForge::Key::Up
#define CF_KEY_PAGE_UP         ::CellForge::Key::PageUp
#define CF_KEY_PAGE_DOWN       ::CellForge::Key::PageDown
#define CF_KEY_HOME            ::CellForge::Key::Home
#define CF_KEY_END             ::CellForge::Key::End
#define CF_KEY_CAPS_LOCK       ::CellForge::Key::CapsLock
#define CF_KEY_SCROLL_LOCK     ::CellForge::Key::ScrollLock
#define CF_KEY_NUM_LOCK        ::CellForge::Key::NumLock
#define CF_KEY_PRINT_SCREEN    ::CellForge::Key::PrintScreen
#define CF_KEY_PAUSE           ::CellForge::Key::Pause
#define CF_KEY_F1              ::CellForge::Key::F1
#define CF_KEY_F2              ::CellForge::Key::F2
#define CF_KEY_F3              ::CellForge::Key::F3
#define CF_KEY_F4              ::CellForge::Key::F4
#define CF_KEY_F5              ::CellForge::Key::F5
#define CF_KEY_F6              ::CellForge::Key::F6
#define CF_KEY_F7              ::CellForge::Key::F7
#define CF_KEY_F8              ::CellForge::Key::F8
#define CF_KEY_F9              ::CellForge::Key::F9
#define CF_KEY_F10             ::CellForge::Key::F10
#define CF_KEY_F11             ::CellForge::Key::F11
#define CF_KEY_F12             ::CellForge::Key::F12
#define CF_KEY_F13             ::CellForge::Key::F13
#define CF_KEY_F14             ::CellForge::Key::F14
#define CF_KEY_F15             ::CellForge::Key::F15
#define CF_KEY_F16             ::CellForge::Key::F16
#define CF_KEY_F17             ::CellForge::Key::F17
#define CF_KEY_F18             ::CellForge::Key::F18
#define CF_KEY_F19             ::CellForge::Key::F19
#define CF_KEY_F20             ::CellForge::Key::F20
#define CF_KEY_F21             ::CellForge::Key::F21
#define CF_KEY_F22             ::CellForge::Key::F22
#define CF_KEY_F23             ::CellForge::Key::F23
#define CF_KEY_F24             ::CellForge::Key::F24
#define CF_KEY_F25             ::CellForge::Key::F25

/* Keypad */
#define CF_KEY_KP_0            ::CellForge::Key::KP0
#define CF_KEY_KP_1            ::CellForge::Key::KP1
#define CF_KEY_KP_2            ::CellForge::Key::KP2
#define CF_KEY_KP_3            ::CellForge::Key::KP3
#define CF_KEY_KP_4            ::CellForge::Key::KP4
#define CF_KEY_KP_5            ::CellForge::Key::KP5
#define CF_KEY_KP_6            ::CellForge::Key::KP6
#define CF_KEY_KP_7            ::CellForge::Key::KP7
#define CF_KEY_KP_8            ::CellForge::Key::KP8
#define CF_KEY_KP_9            ::CellForge::Key::KP9
#define CF_KEY_KP_DECIMAL      ::CellForge::Key::KPDecimal
#define CF_KEY_KP_DIVIDE       ::CellForge::Key::KPDivide
#define CF_KEY_KP_MULTIPLY     ::CellForge::Key::KPMultiply
#define CF_KEY_KP_SUBTRACT     ::CellForge::Key::KPSubtract
#define CF_KEY_KP_ADD          ::CellForge::Key::KPAdd
#define CF_KEY_KP_ENTER        ::CellForge::Key::KPEnter
#define CF_KEY_KP_EQUAL        ::CellForge::Key::KPEqual

#define CF_KEY_LEFT_SHIFT      ::CellForge::Key::LeftShift
#define CF_KEY_LEFT_CONTROL    ::CellForge::Key::LeftControl
#define CF_KEY_LEFT_ALT        ::CellForge::Key::LeftAlt
#define CF_KEY_LEFT_SUPER      ::CellForge::Key::LeftSuper
#define CF_KEY_RIGHT_SHIFT     ::CellForge::Key::RightShift
#define CF_KEY_RIGHT_CONTROL   ::CellForge::Key::RightControl
#define CF_KEY_RIGHT_ALT       ::CellForge::Key::RightAlt
#define CF_KEY_RIGHT_SUPER     ::CellForge::Key::RightSuper
#define CF_KEY_MENU            ::CellForge::Key::Menu

// Mouse
#define CF_MOUSE_BUTTON_LEFT    ::CellForge::Button::Left
#define CF_MOUSE_BUTTON_RIGHT   ::CellForge::Button::Right
#define CF_MOUSE_BUTTON_MIDDLE  ::CellForge::Button::Middle

