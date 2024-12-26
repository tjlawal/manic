// Copyright Frost Gorilla, Inc. All Rights Reserved.

#ifndef OS_GFX_H
#define OS_GFX_H

// Keys
typedef enum OS_Key {
  OS_Key_Null,

  // Letters
  OS_Key_A,
  OS_Key_B,
  OS_Key_C,
  OS_Key_D,
  OS_Key_E,
  OS_Key_F,
  OS_Key_G,
  OS_Key_H,
  OS_Key_I,
  OS_Key_J,
  OS_Key_K,
  OS_Key_L,
  OS_Key_M,
  OS_Key_N,
  OS_Key_O,
  OS_Key_P,
  OS_Key_Q,
  OS_Key_R,
  OS_Key_S,
  OS_Key_T,
  OS_Key_U,
  OS_Key_V,
  OS_Key_W,
  OS_Key_X,
  OS_Key_Y,
  OS_Key_Z,

  // Numbers
  OS_Key_0,
  OS_Key_1,
  OS_Key_2,
  OS_Key_3,
  OS_Key_4,
  OS_Key_5,
  OS_Key_6,
  OS_Key_7,
  OS_Key_8,
  OS_Key_9,

  // Function keys
  OS_Key_F1,
  OS_Key_F2,
  OS_Key_F3,
  OS_Key_F4,
  OS_Key_F5,
  OS_Key_F6,
  OS_Key_F7,
  OS_Key_F8,
  OS_Key_F9,
  OS_Key_F10,
  OS_Key_F11,
  OS_Key_F12,
  OS_Key_F13,
  OS_Key_F14,
  OS_Key_F15,
  OS_Key_F16,
  OS_Key_F17,
  OS_Key_F18,
  OS_Key_F19,
  OS_Key_F20,
  OS_Key_F21,
  OS_Key_F22,
  OS_Key_F23,
  OS_Key_F24,

  // Special keys
  OS_Key_Return,
  OS_Key_Escape,
  OS_Key_Backspace,
  OS_Key_Tab,
  OS_Key_Space,
  OS_Key_CapsLock,

  // Modifiers
  OS_Key_Shift,
  OS_Key_Ctrl,
  OS_Key_Alt,
  OS_Key_Win,

  // Arrow keys
  OS_Key_Left,
  OS_Key_Right,
  OS_Key_Up,
  OS_Key_Down,

  // Navigation cluster
  OS_Key_Insert,
  OS_Key_Delete,
  OS_Key_Home,
  OS_Key_End,
  OS_Key_PageUp,
  OS_Key_PageDown,

  // Numpad
  OS_Key_NumLock,
  OS_Key_NumpadDivide,
  OS_Key_NumpadMultiply,
  OS_Key_NumpadMinus,
  OS_Key_NumpadPlus,
  OS_Key_NumpadDecimal,
  OS_Key_Numpad0,
  OS_Key_Numpad1,
  OS_Key_Numpad2,
  OS_Key_Numpad3,
  OS_Key_Numpad4,
  OS_Key_Numpad5,
  OS_Key_Numpad6,
  OS_Key_Numpad7,
  OS_Key_Numpad8,
  OS_Key_Numpad9,

  // Punctuation/Symbols
  OS_Key_Minus,
  OS_Key_Equals,
  OS_Key_LeftBracket,
  OS_Key_RightBracket,
  OS_Key_Backslash,
  OS_Key_Semicolon,
  OS_Key_Quote,
  OS_Key_Comma,
  OS_Key_Period,
  OS_Key_Slash,
  OS_Key_Tick,

  // Mouse buttons
  OS_Key_LeftMouseButton,
  OS_Key_RightMouseButton,
  OS_Key_MiddleMouseButton,
  OS_Key_MouseButton4,
  OS_Key_MouseButton5,

  // Extended keys, e.g. laptop screen control
  OS_KeyExtend0,
  OS_KeyExtend1,
  OS_KeyExtend2,
  OS_KeyExtend3,
  OS_KeyExtend4,
  OS_KeyExtend5,
  OS_KeyExtend6,
  OS_KeyExtend7,
  OS_KeyExtend8,
  OS_KeyExtend9,
  OS_KeyExtend10,
  OS_KeyExtend11,
  OS_KeyExtend12,
  OS_KeyExtend13,
  OS_KeyExtend14,
  OS_KeyExtend15,
  OS_KeyExtend16,
  OS_KeyExtend17,
  OS_KeyExtend18,
  OS_KeyExtend19,
  OS_KeyExtend20,
  OS_KeyExtend21,
  OS_KeyExtend22,
  OS_KeyExtend23,
  OS_KeyExtend24,
  OS_KeyExtend25,
  OS_KeyExtend26,
  OS_KeyExtend27,
  OS_KeyExtend28,
  OS_KeyExtend29,

  // Count
  OS_Key_COUNT,
} OS_Key;

// Graphical system information
typedef struct OS_GfxInfo OS_GfxInfo;
struct OS_GfxInfo {
  f32 double_click_time;
  f32 caret_blink_time;
  f32 default_refresh_rate;
};

// Window types
typedef u32 OS_WindowFlags;
enum {
  OS_WindowFlag_NormalBorder = (1 << 0),
  OS_WindowFlag_CustomBorder = (1 << 1),
};

// Event types
typedef enum OS_EventKind {
  OS_EventKind_Null,
  OS_EventKind_Press,
  OS_EventKind_Release,
  OS_EventKind_MouseMove,
  OS_EventKind_Text,
  OS_EventKind_Scroll,
  OS_EventKind_WindowLoseFocus,
  OS_EventKind_WindowClose,
  OS_EventKind_Wakeup,
  OS_EventKind_COUNT
} OS_EventKind;

typedef u32 OS_Modifiers;
enum {
  OS_Modifiers_Ctrl = (1 << 0),
  OS_Modifiers_Shift = (1 << 1),
  OS_Modifiers_Alt = (1 << 2),
};

typedef struct OS_Event OS_Event;
struct OS_Event {
  OS_Event *next;
  OS_Event *previous;
  u64 timestamp_us;
  OS_Handle window;
  OS_EventKind event_kind;
  OS_Modifiers key_modifiers;
  OS_Key key;
  b32 is_repeat;
  b32 right_sided;
  b32 left_sided;
  b32 character;
  u32 repeat_count;
  Vec2F32 position;
  Vec2F32 delta;
  string8list strings;
};

typedef struct OS_EventList OS_EventList;
struct OS_EventList {
  u64 count;
  OS_Event *first;
  OS_Event *last;
};

// Application defined frame, forward declaration
internal b32 frame(void);

// Main initialization (Implemented per-os)
internal void os_gfx_init(void);

// Graphics system info (Implemented per-os)
internal OS_GfxInfo *os_get_gfx_info(void);

// Clipboards (Implemented per-os)
internal void os_set_clipboard_text(string8 string);

internal string8 os_get_clipboard_text(Arena *arena);

// ~@os_hooks: Windowing function (Implemented per-os)

internal OS_Handle os_window_open(Vec2S32 window_size, OS_WindowFlags flags, string8 title);
internal void os_window_close(OS_Handle window);
internal void os_window_first_paint(OS_Handle window);
internal void os_window_focus(OS_Handle window);
internal void os_window_set_fullscreen(OS_Handle window, b32 fullscreen);
internal Vec2S32 os_window_dimension(OS_Handle handle);
internal void *os_get_device_context(OS_Handle handle);
internal void os_release_device_context(OS_Handle handle, void *device_context);

// OS Events (Implemented per-os)

internal OS_EventList os_get_events(Arena *arena, b32 wait);
internal OS_Modifiers os_get_modifiers(void);
internal OS_Event *os_event_list_push_new(Arena *arena, OS_EventList *events, OS_EventKind event_kind);

// Native facing graphical message (implemented per-os)
internal void os_graphical_message(b32 error, string8 title, string8 message);

#endif // OS_GFX_H