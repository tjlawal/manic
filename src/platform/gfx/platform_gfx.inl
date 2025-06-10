
#pragma once

namespace Starlight {
	namespace Platform {
		
		// Keys
		enum Key : u32 {
			Key_Null,

			// Letters
			Key_A,
			Key_B,
			Key_C,
			Key_D,
			Key_E,
			Key_F,
			Key_G,
			Key_H,
			Key_I,
			Key_J,
			Key_K,
			Key_L,
			Key_M,
			Key_N,
			Key_O,
			Key_P,
			Key_Q,
			Key_R,
			Key_S,
			Key_T,
			Key_U,
			Key_V,
			Key_W,
			Key_X,
			Key_Y,
			Key_Z,

			// Numbers
			Key_0,
			Key_1,
			Key_2,
			Key_3,
			Key_4,
			Key_5,
			Key_6,
			Key_7,
			Key_8,
			Key_9,

			// Function keys
			Key_F1,
			Key_F2,
			Key_F3,
			Key_F4,
			Key_F5,
			Key_F6,
			Key_F7,
			Key_F8,
			Key_F9,
			Key_F10,
			Key_F11,
			Key_F12,
			Key_F13,
			Key_F14,
			Key_F15,
			Key_F16,
			Key_F17,
			Key_F18,
			Key_F19,
			Key_F20,
			Key_F21,
			Key_F22,
			Key_F23,
			Key_F24,

			// Special keys
			Key_Return,
			Key_Escape,
			Key_Backspace,
			Key_Tab,
			Key_Space,
			Key_CapsLock,

			// Modifiers
			Key_Shift,
			Key_Ctrl,
			Key_Alt,
			Key_Win,

			// Arrow keys
			Key_Left,
			Key_Right,
			Key_Up,
			Key_Down,

			// Navigation cluster
			Key_Insert,
			Key_Delete,
			Key_Home,
			Key_End,
			Key_PageUp,
			Key_PageDown,

			// Numpad
			Key_NumLock,
			Key_NumpadDivide,
			Key_NumpadMultiply,
			Key_NumpadMinus,
			Key_NumpadPlus,
			Key_NumpadDecimal,
			Key_Numpad0,
			Key_Numpad1,
			Key_Numpad2,
			Key_Numpad3,
			Key_Numpad4,
			Key_Numpad5,
			Key_Numpad6,
			Key_Numpad7,
			Key_Numpad8,
			Key_Numpad9,

			// Punctuation/Symbols
			Key_Minus,
			Key_Equals,
			Key_LeftBracket,
			Key_RightBracket,
			Key_Backslash,
			Key_Semicolon,
			Key_Quote,
			Key_Comma,
			Key_Period,
			Key_Slash,
			Key_Tick,

			// Mouse buttons
			Key_LeftMouseButton,
			Key_RightMouseButton,
			Key_MiddleMouseButton,
			Key_MouseButton4,
			Key_MouseButton5,

			// Extended keys
			KeyExtend0,
			KeyExtend1,
			KeyExtend2,
			KeyExtend3,
			KeyExtend4,
			KeyExtend5,
			KeyExtend6,
			KeyExtend7,
			KeyExtend8,
			KeyExtend9,
			KeyExtend10,
			KeyExtend11,
			KeyExtend12,
			KeyExtend13,
			KeyExtend14,
			KeyExtend15,
			KeyExtend16,
			KeyExtend17,
			KeyExtend18,
			KeyExtend19,
			KeyExtend20,
			KeyExtend21,
			KeyExtend22,
			KeyExtend23,
			KeyExtend24,
			KeyExtend25,
			KeyExtend26,
			KeyExtend27,
			KeyExtend28,
			KeyExtend29,

			// Count
			Key_COUNT,
		};

		enum KeyModifiers : u32{
			KeyModifiers_Ctrl = (1 << 0),
			KeyModifiers_Shift = (1 << 1),
			KeyModifiers_Alt = (1 << 2),
		};

		// Event types
		enum EventKind : u32 {
			EventKind_Null,
			EventKind_Press,
			EventKind_Release,
			EventKind_MouseMove,
			EventKind_Text,
			EventKind_Scroll,
			EventKind_WindowLoseFocus,
			EventKind_WindowClose,
			EventKind_Wakeup,
			EventKind_COUNT
		};

		enum AccessFlags : u32 {
			AccessFlag_Read = (1 << 0),
			AccessFlag_Write = (1 << 1),
			AccessFlag_Append = (1 << 2)
		};

		// Window types
		enum WindowFlags : u32 {
			WindowFlag_CustomBorder = (1 << 0),
		};

	}
}
