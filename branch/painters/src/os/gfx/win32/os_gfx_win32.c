// Copyright Frost Gorilla, Inc. All Rights Reserved.

internal OS_W32_Window *os_w32_window_alloc(void) {
  OS_W32_Window *result = os_w32_gfx_state->free_window;
  if (result) {
    SLL_StackPop(os_w32_gfx_state->free_window);
  } else {
    result = push_array_no_zero(os_w32_gfx_state->arena, OS_W32_Window, 1);
  }
  memory_zero_struct(result);
  if (result) {
    DLL_PushBack(os_w32_gfx_state->first_window, os_w32_gfx_state->last_window, result);
  }
  result->last_window_placement.length = sizeof(WINDOWPLACEMENT);
  return result;
}

internal void os_w32_window_release(OS_W32_Window *window) {
  if (window->paint_arena != 0) {
    arena_release(window->paint_arena);
  }
  DestroyWindow(window->hwnd);
  DLL_Remove(os_w32_gfx_state->first_window, os_w32_gfx_state->last_window, window);
  SLL_StackPush(os_w32_gfx_state->free_window, window);
}

// Windows
internal OS_Handle os_w32_handle_from_window(OS_W32_Window *window) {
  OS_Handle handle = {(u64)window};
  return handle;
}

internal OS_W32_Window *os_w32_window_from_handle(OS_Handle handle) {
  OS_W32_Window *window = (OS_W32_Window *)handle.handle[0];
  return window;
}

internal OS_W32_Window *os_w32_window_from_hwnd(HWND hwnd) {
  OS_W32_Window *result = 0;
  for (OS_W32_Window *window = os_w32_gfx_state->first_window; window; window = window->next) {
    if (window->hwnd == hwnd) {
      result = window;
      break;
    }
  }
  return result;
}

internal OS_Handle os_window_open(Vec2S32 window_size, OS_WindowFlags flags, string8 title) {
  HWND hwnd = 0;
  {

    // Arena *arena = arena_alloc();
    // Temp scratch = temp_begin(arena);
    // string16 title16 = str16_from_8(scratch.arena, title);
    hwnd = CreateWindowExW(WS_EX_APPWINDOW, L"graphical-window", (WCHAR *)L"Test will remove",
                           WS_OVERLAPPEDWINDOW | WS_SIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, (int)window_size.x,
                           (int)window_size.y, 0, 0, os_w32_gfx_state->hInstance, 0);

    if (hwnd == NULL) {
      DWORD error = GetLastError();
      // Log or debug print the error
      printf("CreateWindowExW failed with error: %lu\n", error);
    }
    // temp_end(scratch);
  }

  // make window
  OS_W32_Window *window = os_w32_window_alloc();
  {
    window->hwnd = hwnd;
    // TODO(tijani): temporary, do this properly
    window->dpi = 96.f;

    // convert to handle and return
    OS_Handle result = os_w32_handle_from_window(window);
    return result;
  }
}

internal void os_window_close(OS_Handle window_handle) {
  OS_W32_Window *window = os_w32_window_from_handle(window_handle);
  os_w32_window_release(window);
}

internal void os_window_first_paint(OS_Handle window_handle) {
  OS_W32_Window *window = os_w32_window_from_handle(window_handle);
  window->first_paint_done = 1;
  ShowWindow(window->hwnd, SW_SHOW);
  if (window->maximized) {
    ShowWindow(window->hwnd, SW_MAXIMIZE);
  }
}

internal void os_window_focus(OS_Handle window) {}

internal void os_window_set_fullscreen(OS_Handle window, b32 fullscreen) {}

internal Vec2S32 os_window_dimension(OS_Handle handle) {
  OS_W32_Window *window = os_w32_window_from_handle(handle);
  Vec2S32 result = {0};
  RECT rect;

  GetWindowRect(window->hwnd, &rect);
  result.x = (rect.right - rect.left);
  result.y = (rect.bottom - rect.top); // windows start from the top left (0,0) - (x, y).

  return result;
}

internal void *os_get_device_context(OS_Handle handle) {
  OS_W32_Window *window = os_w32_window_from_handle(handle);
  return GetDC(window->hwnd);
}

internal void os_release_device_context(OS_Handle handle, void *device_context) {
  OS_W32_Window *window = os_w32_window_from_handle(handle);
  ReleaseDC(window->hwnd, device_context);
}

internal LRESULT os_w32_window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  LRESULT result = 0;
  b32 good = 1;
  if (os_w32_event_arena == 0) {
    result = DefWindowProcW(hwnd, uMsg, wParam, lParam);
    good = 0;
  }

  if (good) {
    OS_W32_Window *window = os_w32_window_from_hwnd(hwnd);
    OS_Handle window_handle = os_w32_handle_from_window(window);
    b32 release = 0;

    switch (uMsg) {
      case WM_CLOSE: {
        os_w32_push_event(OS_EventKind_WindowClose, window);
      } break;

      case WM_SIZE:
      case WM_PAINT: {
        PAINTSTRUCT paint_struct = {0};
        BeginPaint(hwnd, &paint_struct);
        update();
        EndPaint(hwnd, &paint_struct);
      } break;

        // @TODO(tijani): this is just a stub, might not satisfy our use case.
      case WM_MOUSEMOVE: {
        OS_Event *event = os_w32_push_event(OS_EventKind_MouseMove, window);
        event->position.x = (f32)(s16)LOWORD(lParam);
        event->position.y = (f32)(s16)LOWORD(lParam);
      } break;
      case WM_DPICHANGED: {
        // TODO(tijani): do actual calculation for dpi awareness.
        window->dpi = 96.f;
      } break;

      case WM_KEYDOWN:
      case WM_KEYUP: {
        b32 was_down = (lParam & (1 << 30));
        b32 is_down = (lParam & (1 << 31));

        b32 is_repeat = 0;
        if (!is_down) {
          release = 1;
        } else if (was_down) {
          is_repeat = 1;
        }

        // NOTE(tijani): Making distinction between right sided keys and
        // left-sided, would be useful for key bindings.
        b32 right_sided = 0;
        b32 left_sided = 0;
        if ((lParam & (1 << 24)) && (wParam == VK_RCONTROL || wParam == VK_RMENU || wParam == VK_RSHIFT)) {
          right_sided = 1;
        }
        if ((lParam & (1 << 24)) && (wParam == VK_CONTROL || wParam == VK_MENU || wParam == VK_SHIFT)) {
          left_sided = 1;
        }

        OS_Event *event = os_w32_push_event(release ? OS_EventKind_Release : OS_EventKind_Press, window);
        event->key = os_w32_os_key_from_vkey(wParam);
        // NOTE(tijani): Turn off the top 16 bits, we are only concerned
        // about the bottom 16 to get key repeat count from there.
        event->repeat_count = lParam & (0x0000ffff);
        event->is_repeat = is_repeat;
        event->right_sided = right_sided;
        event->left_sided = left_sided;

        if (event->key == OS_Key_Alt && event->key_modifiers & OS_Modifiers_Alt) {
          event->key_modifiers &= ~OS_Modifiers_Alt;
        }

        if (event->key == OS_Key_Ctrl && event->key_modifiers & OS_Modifiers_Ctrl) {
          event->key_modifiers &= ~OS_Modifiers_Ctrl;
        }

        if (event->key == OS_Key_Shift && event->key_modifiers & OS_Modifiers_Shift) {
          event->key_modifiers &= ~OS_Modifiers_Shift;
        }
      } break;

        // NOTE(tijani): Might want to change this to set custom menu access.
        // Right now it is just doing the traditional windows menu things
        // with ALT+ 'First keyword of the Menu option'.
      case WM_SYSCHAR: {
        result = DefWindowProcW(hwnd, uMsg, wParam, lParam);
      } break;

      default: {
        result = DefWindowProcW(hwnd, uMsg, wParam, lParam);
      } break;
    }
  }

  return result;
}

internal OS_Event *os_w32_push_event(OS_EventKind kind, OS_W32_Window *window) {
  OS_Event *result = os_event_list_push_new(os_w32_event_arena, &os_w32_event_list, kind);
  result->window = os_w32_handle_from_window(window);
  result->key_modifiers = os_get_modifiers();
  return result;
}

internal OS_Key os_w32_os_key_from_vkey(WPARAM virtual_key) {
  local b32 first = 1;
  local OS_Key key_table[256];

  if (first) {
    first = 0;
    memory_zero_array(key_table);

    // Letters
    key_table[(unsigned int)'A'] = OS_Key_A;
    key_table[(unsigned int)'B'] = OS_Key_B;
    key_table[(unsigned int)'C'] = OS_Key_C;
    key_table[(unsigned int)'D'] = OS_Key_D;
    key_table[(unsigned int)'E'] = OS_Key_E;
    key_table[(unsigned int)'F'] = OS_Key_F;
    key_table[(unsigned int)'G'] = OS_Key_G;
    key_table[(unsigned int)'H'] = OS_Key_H;
    key_table[(unsigned int)'I'] = OS_Key_I;
    key_table[(unsigned int)'J'] = OS_Key_J;
    key_table[(unsigned int)'K'] = OS_Key_K;
    key_table[(unsigned int)'L'] = OS_Key_L;
    key_table[(unsigned int)'M'] = OS_Key_M;
    key_table[(unsigned int)'N'] = OS_Key_N;
    key_table[(unsigned int)'O'] = OS_Key_O;
    key_table[(unsigned int)'P'] = OS_Key_P;
    key_table[(unsigned int)'Q'] = OS_Key_Q;
    key_table[(unsigned int)'R'] = OS_Key_R;
    key_table[(unsigned int)'S'] = OS_Key_S;
    key_table[(unsigned int)'T'] = OS_Key_T;
    key_table[(unsigned int)'U'] = OS_Key_U;
    key_table[(unsigned int)'V'] = OS_Key_V;
    key_table[(unsigned int)'W'] = OS_Key_W;
    key_table[(unsigned int)'X'] = OS_Key_X;
    key_table[(unsigned int)'Y'] = OS_Key_Y;
    key_table[(unsigned int)'Z'] = OS_Key_Z;

    // Numbers
    for (u64 i = '0', j = OS_Key_0; i <= '9'; i += 1, j += 1) {
      key_table[i] = (OS_Key)j;
    }

    // Numbers
    for (u64 i = VK_NUMPAD0, j = OS_Key_0; i <= VK_NUMPAD9; i += 1, j += 1) {
      key_table[i] = (OS_Key)j;
    }

    // Function keys: F1-F24
    for (u64 i = VK_F1, j = OS_Key_F1; i <= VK_F24; i += 1, j += 1) {
      key_table[i] = (OS_Key)j;
    }

    // Special keys
    key_table[VK_RETURN] = OS_Key_Return;
    key_table[VK_ESCAPE] = OS_Key_Escape;
    key_table[VK_BACK] = OS_Key_Backspace;
    key_table[VK_TAB] = OS_Key_Tab;
    key_table[VK_SPACE] = OS_Key_Space;
    key_table[VK_CAPITAL] = OS_Key_CapsLock;

    // Modifiers
    // TODO(tijani): differentiate between Left modifiers and right modifiers
    key_table[VK_SHIFT] = OS_Key_Shift;
    key_table[VK_RSHIFT] = OS_Key_Shift;
    key_table[VK_LSHIFT] = OS_Key_Shift;
    key_table[VK_CONTROL] = OS_Key_Ctrl;
    key_table[VK_RCONTROL] = OS_Key_Ctrl;
    key_table[VK_LCONTROL] = OS_Key_Ctrl;
    key_table[VK_MENU] = OS_Key_Alt;
    key_table[VK_RMENU] = OS_Key_Alt;
    key_table[VK_LMENU] = OS_Key_Alt;
    key_table[VK_RWIN] = OS_Key_Win;
    key_table[VK_LWIN] = OS_Key_Win;

    // Arrow keys
    key_table[VK_LEFT] = OS_Key_Left;
    key_table[VK_RIGHT] = OS_Key_Right;
    key_table[VK_UP] = OS_Key_Up;
    key_table[VK_DOWN] = OS_Key_Down;

    // Navigation cluster
    key_table[VK_INSERT] = OS_Key_Insert;
    key_table[VK_DELETE] = OS_Key_Delete;
    key_table[VK_HOME] = OS_Key_Home;
    key_table[VK_END] = OS_Key_End;
    key_table[VK_PRIOR] = OS_Key_PageUp;
    key_table[VK_NEXT] = OS_Key_PageDown;

    // Numpad
    key_table[VK_NUMLOCK] = OS_Key_NumLock;
    key_table[VK_DIVIDE] = OS_Key_NumpadDivide;
    key_table[VK_MULTIPLY] = OS_Key_NumpadMultiply;
    key_table[VK_SUBTRACT] = OS_Key_NumpadMinus;
    key_table[VK_ADD] = OS_Key_NumpadPlus;
    key_table[VK_DECIMAL] = OS_Key_NumpadDecimal;

    // Numpad Numbers
    for (u32 i = 0; i < 10; i += 1) {
      key_table[VK_NUMPAD0 + i] = (OS_Key)((u64)(OS_Key_Numpad0 + i));
    }

    // Punctuation/Symbols
    key_table[VK_OEM_MINUS] = OS_Key_Minus;
    key_table[VK_OEM_PLUS] = OS_Key_Equals;
    key_table[VK_OEM_1] = OS_Key_Semicolon;
    key_table[VK_OEM_2] = OS_Key_Slash;
    key_table[VK_OEM_3] = OS_Key_Tick;
    key_table[VK_OEM_4] = OS_Key_LeftBracket;
    key_table[VK_OEM_5] = OS_Key_Backslash;
    key_table[VK_OEM_6] = OS_Key_RightBracket;
    key_table[VK_OEM_7] = OS_Key_Quote;
    key_table[VK_OEM_COMMA] = OS_Key_Comma;
    key_table[VK_OEM_PERIOD] = OS_Key_Period;

    // NOTE(tijani): These extended keys are for edge cases
    // with computers with custom hardware that our software
    // runs on. We must handle these keycodes too even though
    // we do not know what they are.
    // Cheking bit values 223 upto but not including 255
    for (u64 i = 0xDF, j = 0; i < 0xFF; i += 1, j += 1) {
      key_table[i] = (OS_Key)(OS_KeyExtend0 + j);
    }
  }
  // NOTE(tijani): Ensure virtual_key is only mapped no the
  // lowest 8-bits of WPARAM. It also ensures we don't tramp
  // on other bit fields of WPARAM since we have no business
  // with them in this context, 256 is all we need.
  OS_Key key = key_table[virtual_key & (0x000000ff)];
  return key;
}
internal WPARAM os_w32_vkey_from_os_key(OS_Key key) {
  // TODO(tijani)
  return 0;
}

//////////////////////////////
// OS Events (Implemented per-os)

internal OS_EventList os_get_events(Arena *arena, b32 wait) {
  os_w32_event_arena = arena;
  memory_zero_struct(&os_w32_event_list);
  MSG msg = {0};

  if (!wait || GetMessage(&msg, 0, 0, 0)) {
    b32 first_wait = wait;
    for (; first_wait || PeekMessage(&msg, 0, 0, 0, PM_REMOVE); first_wait = 0) {
      DispatchMessage(&msg);
      TranslateMessage(&msg);
      if (msg.message == WM_QUIT) {
        os_w32_push_event(OS_EventKind_WindowClose, 0);
      }
    }
  }

  return os_w32_event_list;
}

internal OS_Modifiers os_get_modifiers(void) {
  OS_Modifiers modifiers = 0;
  if (GetKeyState(VK_CONTROL) & 0x8000) {
    modifiers |= OS_Modifiers_Ctrl;
  }
  if (GetKeyState(VK_SHIFT) & 0x8000) {
    modifiers |= OS_Modifiers_Shift;
  }

  if (GetKeyState(VK_MENU) & 0x8000) {
    modifiers |= OS_Modifiers_Alt;
  }
  return modifiers;
}

// Native facing graphical message (implemented per-os)
internal void os_graphical_message(b32 error, string8 title, string8 message) {
  // Arena *arena = arena_alloc(.reserve_size = MB(1));
  // Temp scratch = temp_begin(arena);
  // string16 title16 = str16_from_8(scratch.arena, title);
  // string16 message16 = str16_from_8(scratch.arena, message);
  // MessageBoxW(0, (WCHAR *)message16.str, (WCHAR *)title16.str, MB_OK | (!!error * MB_ICONERROR));
  // scratch_end(scratch);

  MessageBoxW(0, (WCHAR *)L"Error Occured", (WCHAR *)L"Error!!", MB_OK | MB_ICONERROR);
}

// Main initialization (Implemented per-os)
internal void os_gfx_init(void) {
  // set up base share state
  Arena *arena = arena_alloc();
  os_w32_gfx_state = push_array(arena, OS_W32_GfxState, 1);
  os_w32_gfx_state->arena = arena;
  os_w32_gfx_state->gfx_thread_id = (u32)GetCurrentThreadId();
  os_w32_gfx_state->hInstance = GetModuleHandle(0);

  // @TODO: this might be too windows 10 specific. research if that is the case and find a better way to handle it.
  // Set DPI Awareness for the entire application, this applies to all related threads.
  SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

  // register graphical window class
  {
    WNDCLASSEXW wndclass = {sizeof(wndclass)};
    wndclass.lpfnWndProc = os_w32_window_proc;
    wndclass.hInstance = os_w32_gfx_state->hInstance;
    wndclass.lpszClassName = L"graphical-window";
    wndclass.hCursor = LoadCursor(0, IDC_ARROW);
    wndclass.hIcon = LoadIcon(os_w32_gfx_state->hInstance, MAKEINTRESOURCE(1));
    wndclass.style = CS_VREDRAW | CS_HREDRAW;

    ATOM wndatom = RegisterClassExW(&wndclass);
    (void)wndatom;
  }
}

// Graphics system info (Implemented per-os)
internal OS_GfxInfo *os_get_gfx_info(void) { return &os_w32_gfx_state->gfx_info; }

// Clipboards (Implemented per-os)
// internal void os_set_clipboard_text(string8 string) {}
//
// internal string8 os_get_clipboard_text(Arena *arena) {}
