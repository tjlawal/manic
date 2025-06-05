
using namespace Starlight::Foundation;

namespace Starlight {
	namespace Platform {
		namespace Gfx {

				Window *w32_window_alloc(void) {
					ProfBlock(0, profDebug_darkmagenta);

					Window *result = w32_gfx_state->free_window;
					if (result) {
						SLLPop(w32_gfx_state->free_window);
					} else {
						//result = push_array_no_zero(w32_gfx_state->arena, Window, 1);
						result = arena_push_non_zeroed_aligned<Window>(w32_gfx_state->arena, 1);
					}
					MemoryZeroStruct(result);
					if (result) {
						DLL_PushBack(w32_gfx_state->first_window, w32_gfx_state->last_window, result);
					}
					result->window_placement.length = sizeof(WINDOWPLACEMENT);
					return result;
				}

				void w32_window_release(Window *window) {
					if (window->paint_arena != 0) {
						arena_release(window->paint_arena);
					}
					DestroyWindow(window->hwnd);
					DLL_Remove(w32_gfx_state->first_window, w32_gfx_state->last_window, window);
					SLLPush(w32_gfx_state->free_window, window);
				}

				// Windows
				Handle w32_handle_from_window(Window *window) {
					ProfBlock(0, profDebug_darkmagenta);

					Handle handle = {(u64)window};
					return handle;
				}

				Window *w32_window_from_handle(Handle handle) {
					Window *window = (Window *)handle.handle[0];
					return window;
				}

				Window *w32_window_from_hwnd(HWND hwnd) {
					Window *result = 0;
					for (Window *window = w32_gfx_state->first_window; window; window = window->next) {
						if (window->hwnd == hwnd) {
							result = window;
							break;
						}
					}
					return result;
				}

				Handle window_open(Vec2s window_size, string8 title) {
					ProfBlock(0, profDebug_darkmagenta);

					HWND hwnd = 0;
					{
						Temp scratch = scratch_begin(0, 0);
						string16 title16 = str16_from_8(scratch.arena, title);
						hwnd = CreateWindowExW(WS_EX_APPWINDOW, L"graphical-window", (WCHAR *)title16.str, WS_OVERLAPPEDWINDOW | WS_SIZEBOX,
																	 CW_USEDEFAULT, CW_USEDEFAULT, (int)window_size.x, (int)window_size.y, 0, 0,
																	 w32_gfx_state->hInstance, 0);

						if (hwnd == NULL) {
							DWORD error = GetLastError();
							// Log or debug print the error
							printf("CreateWindowExW failed with error: %lu\n", error);
						}
						scratch_end(scratch);
					}

					// make window
					Window *window = w32_window_alloc();
					{
						window->hwnd = hwnd;
						// TODO(tijani): temporary, do this properly
						window->dpi = 96.f;

						// convert to handle and return
						Handle result = w32_handle_from_window(window);
						return result;
					}
				}

				void window_close(Handle window_handle) {
					Window *window = w32_window_from_handle(window_handle);
					w32_window_release(window);
				}

				void window_first_paint(Handle window_handle) {
					ProfBlock(0, profDebug_darkmagenta);

					Window *window = w32_window_from_handle(window_handle);
					window->first_paint = 1;
					ShowWindow(window->hwnd, SW_SHOW);
					if (window->maximized) {
						ShowWindow(window->hwnd, SW_MAXIMIZE);
					}
				}

				void window_focus(Handle window) {}

				void window_set_fullscreen(Handle window, b32 fullscreen) {}

				Vec2s get_window_dimension(Handle handle) {
					ProfBlock(0, profDebug_darkmagenta);

					Window *window = w32_window_from_handle(handle);
					Vec2s result = {};
					RECT rect;

					GetWindowRect(window->hwnd, &rect);
					result.x = (rect.right - rect.left);
					result.y = (rect.bottom - rect.top); // windows start from the top left (0,0) - (x, y).

					return result;
				}

				void *get_device_context(Handle handle) {
					ProfBlock(0, profDebug_darkmagenta);

					Window *window = w32_window_from_handle(handle);
					return GetDC(window->hwnd);
				}

				void release_device_context(Handle handle, void *device_context) {
					Window *window = w32_window_from_handle(handle);
					ReleaseDC(window->hwnd, static_cast<HDC>(device_context));
				}

				LRESULT w32_window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
					ProfBlock(0, profDebug_darkmagenta);

					LRESULT result = 0;
					b32 good = 1;
					if (w32_event_arena == 0) {
						result = DefWindowProcW(hwnd, uMsg, wParam, lParam);
						good = 0;
					}

					if (good) {
						Window *window = w32_window_from_hwnd(hwnd);
						Handle window_handle = w32_handle_from_window(window);
						b32 release = 0;

						switch (uMsg) {
							case WM_CLOSE: {
								w32_push_event(EventKind_WindowClose, window);
							} break;

							case WM_SIZE:
							case WM_PAINT: {
								PAINTSTRUCT paint_struct = {0};
								BeginPaint(hwnd, &paint_struct);
								update();
								//render();
								EndPaint(hwnd, &paint_struct);
							} break;

							case WM_MOUSEMOVE: {
								// x-coords is the LOWORD, y-coords is in HIWORD
								Event *event = w32_push_event(EventKind_MouseMove, window);
								event->position.x = (f32)(s16)LOWORD(lParam); // Horizontal position
								event->position.y = (f32)(s16)HIWORD(lParam); // Vertical position
							} break;

							case WM_DPICHANGED: {
								f32 new_dpi = (f32)(wParam & 0xffff);
								window->dpi = new_dpi;
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

								// Making distinction between right sided keys and
								// left-sided, could be useful for key bindings??
								b32 right_sided = 0;
								b32 left_sided = 0;
								if ((lParam & (1 << 24)) && (wParam == VK_RCONTROL || wParam == VK_RMENU || wParam == VK_RSHIFT)) {
									right_sided = 1;
								}
								if ((lParam & (1 << 24)) && (wParam == VK_CONTROL || wParam == VK_MENU || wParam == VK_SHIFT)) {
									left_sided = 1;
								}

								Event *event = w32_push_event(release ? EventKind_Release : EventKind_Press, window);
								event->key = w32_os_key_from_vkey(wParam);
								// Turn off the top 16 bits, we are only concerned
								// about the bottom 16 to get key repeat count from there.
								event->repeat_count = lParam & (0x0000ffff);
								event->is_repeat = is_repeat;
								event->right_sided = right_sided;
								event->left_sided = left_sided;

								if (event->key == Key_Alt && event->key_modifiers & KeyModifiers_Alt) {
									event->key_modifiers = static_cast<KeyModifiers>(event->key_modifiers & ~KeyModifiers_Alt);
								}

								if (event->key == Key_Ctrl && event->key_modifiers & KeyModifiers_Ctrl) {
									event->key_modifiers = static_cast<KeyModifiers>(event->key_modifiers & ~KeyModifiers_Ctrl);
								}

								if (event->key == Key_Shift && event->key_modifiers & KeyModifiers_Shift) {
									event->key_modifiers = static_cast<KeyModifiers>(event->key_modifiers & ~KeyModifiers_Shift);
								}
							} break;

							// Might want to change this to set custom menu access.
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

				Event *w32_push_event(EventKind kind, Window *window) {
					ProfBlock(0, profDebug_darkmagenta);

					Event *result = event_list_push_new(w32_event_arena, &w32_event_list, kind);
					result->window = w32_handle_from_window(window);
					result->key_modifiers = get_modifiers();
					return result;
				}

				Key w32_os_key_from_vkey(WPARAM virtual_key) {
					ProfBlock(0, profDebug_darkmagenta);

					local b32 first = 1;
					local Key key_table[256];

					if (first) {
						first = 0;
						MemoryZeroArray(key_table);

						// Letters
						key_table[(unsigned int)'A'] = Key_A;
						key_table[(unsigned int)'B'] = Key_B;
						key_table[(unsigned int)'C'] = Key_C;
						key_table[(unsigned int)'D'] = Key_D;
						key_table[(unsigned int)'E'] = Key_E;
						key_table[(unsigned int)'F'] = Key_F;
						key_table[(unsigned int)'G'] = Key_G;
						key_table[(unsigned int)'H'] = Key_H;
						key_table[(unsigned int)'I'] = Key_I;
						key_table[(unsigned int)'J'] = Key_J;
						key_table[(unsigned int)'K'] = Key_K;
						key_table[(unsigned int)'L'] = Key_L;
						key_table[(unsigned int)'M'] = Key_M;
						key_table[(unsigned int)'N'] = Key_N;
						key_table[(unsigned int)'O'] = Key_O;
						key_table[(unsigned int)'P'] = Key_P;
						key_table[(unsigned int)'Q'] = Key_Q;
						key_table[(unsigned int)'R'] = Key_R;
						key_table[(unsigned int)'S'] = Key_S;
						key_table[(unsigned int)'T'] = Key_T;
						key_table[(unsigned int)'U'] = Key_U;
						key_table[(unsigned int)'V'] = Key_V;
						key_table[(unsigned int)'W'] = Key_W;
						key_table[(unsigned int)'X'] = Key_X;
						key_table[(unsigned int)'Y'] = Key_Y;
						key_table[(unsigned int)'Z'] = Key_Z;

						// Numbers
						for (u64 i = '0', j = Key_0; i <= '9'; i += 1, j += 1) {
							key_table[i] = (Key)j;
						}

						// Numbers
						for (u64 i = VK_NUMPAD0, j = Key_0; i <= VK_NUMPAD9; i += 1, j += 1) {
							key_table[i] = (Key)j;
						}

						// Function keys: F1-F24
						for (u64 i = VK_F1, j = Key_F1; i <= VK_F24; i += 1, j += 1) {
							key_table[i] = (Key)j;
						}

						// Special keys
						key_table[VK_RETURN] = Key_Return;
						key_table[VK_ESCAPE] = Key_Escape;
						key_table[VK_BACK] = Key_Backspace;
						key_table[VK_TAB] = Key_Tab;
						key_table[VK_SPACE] = Key_Space;
						key_table[VK_CAPITAL] = Key_CapsLock;

						// Modifiers
						// TODO(tijani): differentiate between Left modifiers and right modifiers
						key_table[VK_SHIFT] = Key_Shift;
						key_table[VK_RSHIFT] = Key_Shift;
						key_table[VK_LSHIFT] = Key_Shift;
						key_table[VK_CONTROL] = Key_Ctrl;
						key_table[VK_RCONTROL] = Key_Ctrl;
						key_table[VK_LCONTROL] = Key_Ctrl;
						key_table[VK_MENU] = Key_Alt;
						key_table[VK_RMENU] = Key_Alt;
						key_table[VK_LMENU] = Key_Alt;
						key_table[VK_RWIN] = Key_Win;
						key_table[VK_LWIN] = Key_Win;

						// Arrow keys
						key_table[VK_LEFT] = Key_Left;
						key_table[VK_RIGHT] = Key_Right;
						key_table[VK_UP] = Key_Up;
						key_table[VK_DOWN] = Key_Down;

						// Navigation cluster
						key_table[VK_INSERT] = Key_Insert;
						key_table[VK_DELETE] = Key_Delete;
						key_table[VK_HOME] = Key_Home;
						key_table[VK_END] = Key_End;
						key_table[VK_PRIOR] = Key_PageUp;
						key_table[VK_NEXT] = Key_PageDown;

						// Numpad
						key_table[VK_NUMLOCK] = Key_NumLock;
						key_table[VK_DIVIDE] = Key_NumpadDivide;
						key_table[VK_MULTIPLY] = Key_NumpadMultiply;
						key_table[VK_SUBTRACT] = Key_NumpadMinus;
						key_table[VK_ADD] = Key_NumpadPlus;
						key_table[VK_DECIMAL] = Key_NumpadDecimal;

						// Numpad Numbers
						for (u32 i = 0; i < 10; i += 1) {
							key_table[VK_NUMPAD0 + i] = (Key)((u64)(Key_Numpad0 + i));
						}

						// Punctuation/Symbols
						key_table[VK_OEM_MINUS] = Key_Minus;
						key_table[VK_OEM_PLUS] = Key_Equals;
						key_table[VK_OEM_1] = Key_Semicolon;
						key_table[VK_OEM_2] = Key_Slash;
						key_table[VK_OEM_3] = Key_Tick;
						key_table[VK_OEM_4] = Key_LeftBracket;
						key_table[VK_OEM_5] = Key_Backslash;
						key_table[VK_OEM_6] = Key_RightBracket;
						key_table[VK_OEM_7] = Key_Quote;
						key_table[VK_OEM_COMMA] = Key_Comma;
						key_table[VK_OEM_PERIOD] = Key_Period;

						// These extended keys are for edge cases
						// with computers with custom hardware that our software
						// runs on. We must handle these keycodes too even though
						// we do not know what they are.
						// Cheking bit values 223 upto but not including 255
						for (u64 i = 0xDF, j = 0; i < 0xFF; i += 1, j += 1) {
							key_table[i] = (Key)(KeyExtend0 + j);
						}
					}
					// Ensure virtual_key is only mapped to the
					// lowest 8-bits of WPARAM. It also ensures we don't tramp
					// on other bit fields of WPARAM since we have no business
					// with them in this context, 256 is all we need.
					Key key = key_table[virtual_key & (bitmask8)];
					return key;
				}
	
				WPARAM w32_vkey_from_key(Key key) {
					// TODO(tijani)
					return 0;
				}

				//////////////////////////////
				// OS Events

				EventList get_events(Arena* arena, b32 wait) {
					ProfBlock(0, profDebug_darkmagenta);

					w32_event_arena = arena;
					MemoryZeroStruct(&w32_event_list);
					MSG msg = {0};

					if (!wait || GetMessage(&msg, 0, 0, 0)) {
						b32 first_wait = wait;
						for (; first_wait || PeekMessage(&msg, 0, 0, 0, PM_REMOVE); first_wait = 0) {
							DispatchMessage(&msg);
							TranslateMessage(&msg);
							if (msg.message == WM_QUIT) {
								w32_push_event(EventKind_WindowClose, 0);
							}
						}
					}

					return w32_event_list;
				}

				KeyModifiers get_modifiers(void) {
					ProfBlock(0, profDebug_darkmagenta);

					KeyModifiers modifiers = {};
					if (GetKeyState(VK_CONTROL) & 0x8000) {
						modifiers = static_cast<KeyModifiers>(modifiers | KeyModifiers_Ctrl);
					}
					if (GetKeyState(VK_SHIFT) & 0x8000) {
						modifiers = static_cast<KeyModifiers>(modifiers | KeyModifiers_Shift);
					}

					if (GetKeyState(VK_MENU) & 0x8000) {
						modifiers = static_cast<KeyModifiers>(modifiers | KeyModifiers_Alt);
					}
					return modifiers;
				}

				// Native facing graphical message (implemented per-os)
				void graphical_message(b32 error, string8 title, string8 message) {
					Temp scratch = scratch_begin(0, 0);

					string16 title16 = str16_from_8(scratch.arena, title);
					string16 message16 = str16_from_8(scratch.arena, message);
					// The bit trick here is a shortcut for if/else.
					// It checks if error is 0, if it is then it just displays MB_OK else it displays MB_ICONERROR * the error.
					MessageBoxW(0, (WCHAR *)message16.str, (WCHAR *)title16.str, MB_OK | (!!error * MB_ICONERROR));
					scratch_end(scratch);
				}

				// Main gfx initialization (Implemented per-os)
				void gfx_init(void) {
					ProfBlock(0, profDebug_darkmagenta);

					Arena *arena = arena_alloc();
					w32_gfx_state = arena_push<GfxState>(arena, 1);
					w32_gfx_state->arena = arena;
					w32_gfx_state->gfx_thread_id = (u32)GetCurrentThreadId();
					w32_gfx_state->hInstance = GetModuleHandle(0);

					// TODO(tijani): this might be too windows 10 specific. research if that is
					// the case and find a better way to handle it. Set DPI Awareness for the
					// entire application, this applies to all related threads.
					SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

					// register graphical window class
					{
						WNDCLASSEXW wndclass = {sizeof(wndclass)};
						wndclass.lpfnWndProc = w32_window_proc;
						wndclass.hInstance = w32_gfx_state->hInstance;
						wndclass.lpszClassName = L"graphical-window";
						wndclass.hCursor = LoadCursor(0, IDC_ARROW);
						wndclass.hIcon = LoadIcon(w32_gfx_state->hInstance, MAKEINTRESOURCE(1));
						wndclass.style = CS_VREDRAW | CS_HREDRAW;

						ATOM wndatom = RegisterClassExW(&wndclass);
						(void)wndatom;
					}
				}

				// Graphics system info (Implemented per-os)
				GfxInfo *get_gfx_info(void) { return &w32_gfx_state->gfx_info; }
		}
	}
}