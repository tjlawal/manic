// Copyright (c) 2024 Frost Gorilla, Inc. All Rights Reserved.

#ifndef OS_GFX_WIN32_H
#define OS_GFX_WIN32_H

typedef struct OS_W32_Window OS_W32_Window;
struct OS_W32_Window {
  OS_W32_Window *next;
  OS_W32_Window *previous;
  HWND hwnd;
  WINDOWPLACEMENT last_window_placement;
  f32 dpi;
  b32 first_paint_done;
  b32 maximized;
  Arena *paint_arena;
};

// Global State
typedef struct OS_W32_GfxState OS_W32_GfxState;
struct OS_W32_GfxState {
  Arena *arena;
  u32 gfx_thread_id;
  HINSTANCE hInstance;
  HCURSOR hCursor;
  OS_GfxInfo gfx_info;
  OS_W32_Window *first_window;
  OS_W32_Window *last_window;
  OS_W32_Window *free_window;
  OS_Key key_from_vkey_table[256];
};

// Globals
global OS_W32_GfxState *os_w32_gfx_state = 0;
global OS_EventList os_w32_event_list = {0};
global Arena *os_w32_event_arena = 0;
b32 os_w32_resizing = 0;

// Window specific functions. These are specific to WIN32
internal OS_Handle os_w32_handle_from_window(OS_W32_Window *window);
internal OS_W32_Window *os_w32_window_from_handle(OS_Handle window);

internal HWND os_w32_hwnd_from_window(OS_W32_Window *window);
internal OS_W32_Window *os_w32_window_from_hwnd(HWND hwnd);

internal OS_W32_Window *os_w32_window_alloc(void);
internal void os_w32_window_release(OS_W32_Window *window);
internal LRESULT os_w32_window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

internal OS_Event *os_w32_push_event(OS_EventKind kind, OS_W32_Window *window);
internal OS_Key os_w32_os_key_from_vkey(WPARAM virtual_key);
internal WPARAM os_w32_vkey_from_os_key(OS_Key key);

#endif // OS_GFX_WIN32_H