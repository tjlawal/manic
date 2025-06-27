
using namespace Starlight::Foundation;

namespace Starlight {
	namespace Platform {
		namespace Gfx {

			///////////
			// Helpers
			// WIN32 to/from platform layer windowing functions
			Handle w32_handle_from_window(Window *window) {
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

			HWND w32_hwnd_from_window(Window* window) { return window->hwnd; }

			Rng2f32 w32_rng_from_rect(RECT rect){
				Rng2f32	r = {};
				r.x0 = static_cast<f32>(rect.left);
				r.x1 = static_cast<f32>(rect.right);
				r.y0 = static_cast<f32>(rect.top);
				r.y1 = static_cast<f32>(rect.bottom);
				return r;
			}

			Window *w32_window_alloc(void) {
				ProfFunction(profDebug_powderblue);

				Window *result = w32_gfx_state->free_window;
				if (result)
					SLLPop(w32_gfx_state->free_window);
				else 
					result = arena_push_non_zeroed<Window>(w32_gfx_state->arena, 1);
				MemoryZeroStruct(result);

				if (result) 
					DLL_PushBack(w32_gfx_state->first_window, w32_gfx_state->last_window, result);
				result->window_placement.length = sizeof(WINDOWPLACEMENT);
				return result;
			}

			void w32_window_release(Window *window) {
				DestroyWindow(window->hwnd);
				DLL_Remove(w32_gfx_state->first_window, w32_gfx_state->last_window, window);
				SLLPush(w32_gfx_state->free_window, window);
			}

			Handle window_open(Rng2f32 window_size, string8 title) {
				ProfFunction(profDebug_powderblue);

				HWND hwnd = 0;
				Vec2f window_dim = dim2f32(window_size);

				{
					Temp scratch = scratch_begin(0, 0);
					string16 title16 = str16_from_8(scratch.arena, title);
					hwnd = CreateWindowExW(WS_EX_APPWINDOW, L"graphical-window", (WCHAR *)title16.str, 
																 (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX), 
																 CW_USEDEFAULT, CW_USEDEFAULT, static_cast<s32>(window_dim.x), 
																 static_cast<s32>(window_dim.y), 0, 0, w32_gfx_state->hInstance, 0);

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
					window->hdc = GetDC(window->hwnd);
					window->dpi = GetDpiForWindow(hwnd);;
				}

				// convert to handle and return
				Handle result = w32_handle_from_window(window);
				return result;
			}

			void window_close(Handle window_handle) {
				Window *window = w32_window_from_handle(window_handle);
				w32_window_release(window);
			}

			void window_first_paint(Handle window_handle) {
				ProfFunction(profDebug_indianred);
				Window *window = w32_window_from_handle(window_handle);
				window->first_paint = 1;
				ShowWindow(window->hwnd, SW_SHOW);
				if (window->maximized) {
					ShowWindow(window->hwnd, SW_MAXIMIZE);
				}
			}

			Rng2f32 rect_from_window(Handle window_handle) {
				Rng2f32 range = {};
				Window *w = w32_window_from_handle(window_handle);
				if(w) {
					RECT r = {};
					GetWindowRect(w->hwnd, &r);
					range = w32_rng_from_rect(r);
				}
				return range;
			}

			Rng2f32 client_rect_from_window(Handle window_handle) {
				Rng2f32 range = {};
				Window *w = w32_window_from_handle(window_handle);
				if(w) {
					RECT rect = {};
					GetClientRect(w->hwnd, &rect);
					range = w32_rng_from_rect(rect);
				}
				return range;
			}

			LRESULT w32_window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
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
							event->key = key_from_w32_vkey(wParam);
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
				ProfFunction(profDebug_powderblue);
				Event *result = event_list_push_new(w32_event_arena, &w32_event_list, kind);
				result->window = w32_handle_from_window(window);
				result->key_modifiers = get_modifiers();
				return result;
			}

			Key key_from_w32_vkey(WPARAM virtual_key) {
				ProfFunction(profDebug_powderblue);
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
				WPARAM res = 0;
				{
					local b32 inited = 0;
					local WPARAM vkey_table[Key_COUNT] = {0};

					if(inited == 0) {
						inited = 1;
						vkey_table[Key_Escape] = VK_ESCAPE;
						for(Key key = Key_F1; key < Key_F24; key = static_cast<Key>(key+1)) {
							vkey_table[key] = VK_F1 + (key - Key_F1);
						}

						for(Key key = Key_0; key <= Key_9; key = static_cast<Key>(key+1)) {
							vkey_table[key] = '0' + (key - Key_0);
						}

						// Punctuation/Symbols
						vkey_table[Key_Minus] 						= VK_OEM_MINUS;
						vkey_table[Key_Equals] 						= VK_OEM_PLUS;
						vkey_table[Key_LeftBracket] 			= VK_OEM_4;
						vkey_table[Key_RightBracket] 			= VK_OEM_6;
						vkey_table[Key_Backslash] 				= VK_OEM_5;
						vkey_table[Key_Semicolon] 				= VK_OEM_1;
						vkey_table[Key_Quote] 						= VK_OEM_7;
						vkey_table[Key_Slash]							= VK_OEM_2;
						vkey_table[Key_Comma]							= VK_OEM_COMMA;
						vkey_table[Key_Period]						= VK_OEM_PERIOD;
						vkey_table[Key_Tick] 							= VK_OEM_3;

						// Special Keys
						vkey_table[Key_Return]						= VK_RETURN;
						vkey_table[Key_Backspace] 				= VK_BACK;
						vkey_table[Key_Tab] 							= VK_TAB;
						vkey_table[Key_Space] 						= VK_SPACE;
						vkey_table[Key_CapsLock] 					= VK_CAPITAL;

						// Modifiers
						vkey_table[Key_Shift]							= VK_SHIFT;
						vkey_table[Key_Ctrl]							= VK_CONTROL;
						vkey_table[Key_Ctrl]							= VK_LCONTROL;
						vkey_table[Key_Alt] 							= VK_MENU;
						vkey_table[Key_Win]								= VK_RWIN;

						// Arrow Keys
						vkey_table[Key_Up] 								= VK_UP;
						vkey_table[Key_Left] 							= VK_LEFT;
						vkey_table[Key_Down] 							= VK_DOWN;
						vkey_table[Key_Right] 						= VK_RIGHT;

						// Navigation cluster
						vkey_table[Key_Insert] 						= VK_INSERT;
						vkey_table[Key_Delete] 						= VK_DELETE;
						vkey_table[Key_Home] 							= VK_HOME;
						vkey_table[Key_End] 							= VK_END;
						vkey_table[Key_PageUp] 						= VK_PRIOR;
						vkey_table[Key_PageDown] 					= VK_NEXT;

						// Numpad
						vkey_table[Key_NumLock] 					= VK_NUMLOCK;
						vkey_table[Key_Numpad0] 					= VK_NUMPAD0;
						vkey_table[Key_Numpad1] 					= VK_NUMPAD1;
						vkey_table[Key_Numpad2] 					= VK_NUMPAD2;
						vkey_table[Key_Numpad3] 					= VK_NUMPAD3;
						vkey_table[Key_Numpad4] 					= VK_NUMPAD4;
						vkey_table[Key_Numpad5] 					= VK_NUMPAD5;
						vkey_table[Key_Numpad6] 					= VK_NUMPAD6;
						vkey_table[Key_Numpad7] 					= VK_NUMPAD7;
						vkey_table[Key_Numpad8] 					= VK_NUMPAD8;
						vkey_table[Key_Numpad9] 					= VK_NUMPAD9;
						vkey_table[Key_NumpadDivide] 			= VK_DIVIDE;
						vkey_table[Key_NumpadMultiply]		= VK_MULTIPLY;
						vkey_table[Key_NumpadMinus] 			= VK_SUBTRACT;
						vkey_table[Key_NumpadPlus] 				= VK_ADD;
						vkey_table[Key_NumpadDecimal] 		= VK_DECIMAL;

						// Mouse buttons
						vkey_table[Key_LeftMouseButton] 	= VK_LBUTTON;
						vkey_table[Key_RightMouseButton] 	= VK_RBUTTON;
						vkey_table[Key_MiddleMouseButton] = VK_MBUTTON;
						vkey_table[Key_MouseButton4]			= VK_XBUTTON1;
						vkey_table[Key_MouseButton5]			= VK_XBUTTON2;

						// Letters
						vkey_table[Key_A] 								= 'A';
						vkey_table[Key_B] 								= 'B';
						vkey_table[Key_C] 								= 'C';
						vkey_table[Key_D] 								= 'D';
						vkey_table[Key_E] 								= 'E';
						vkey_table[Key_F] 								= 'F';
						vkey_table[Key_G] 								= 'G';
						vkey_table[Key_H] 								= 'H';
						vkey_table[Key_I] 								= 'I';
						vkey_table[Key_J] 								= 'J';
						vkey_table[Key_K] 								= 'K';
						vkey_table[Key_L] 								= 'L';
						vkey_table[Key_M] 								= 'M';
						vkey_table[Key_N] 								= 'N';
						vkey_table[Key_O] 								= 'O';
						vkey_table[Key_P] 								= 'P';
						vkey_table[Key_Q] 								= 'Q';
						vkey_table[Key_R] 								= 'R';
						vkey_table[Key_S] 								= 'S';
						vkey_table[Key_T] 								= 'T';
						vkey_table[Key_U] 								= 'U';
						vkey_table[Key_V] 								= 'V';
						vkey_table[Key_W] 								= 'W';
						vkey_table[Key_X] 								= 'X';
						vkey_table[Key_Y] 								= 'Y';
						vkey_table[Key_Z] 								= 'Z';
					}
					res = vkey_table[key];
				}
				return res;
			}

			//////////////////////////////
			// OS Events
			EventList get_events(Arena* arena, b32 wait) {
				ProfFunction(profDebug_powderblue);
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
				ProfFunction(profDebug_powderblue);
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

			// OS gfx layer message box
			void graphical_message(b32 error, string8 title, string8 message) {
				Temp scratch = scratch_begin(0, 0);

				string16 title16 = str16_from_8(scratch.arena, title);
				string16 message16 = str16_from_8(scratch.arena, message);
				// The bit trick here is a shortcut for if/else.
				// It checks if error is 0, if it is then it just displays MB_OK else it displays MB_ICONERROR * the error.
				MessageBoxW(0, (WCHAR *)message16.str, (WCHAR *)title16.str, MB_OK | (!!error * MB_ICONERROR));
				scratch_end(scratch);
			}

			// Main gfx layer initialization
			void gfx_init(void) {
				ProfFunction(profDebug_powderblue);

				Arena *arena = arena_alloc();
				w32_gfx_state = arena_push<GfxState>(arena, 1);
				w32_gfx_state->arena = arena;
				w32_gfx_state->gfx_thread_id = (u32)GetCurrentThreadId();
				w32_gfx_state->hInstance = GetModuleHandle(0);

				/// Set dpi awareness. Note is to be taken that this doesn't support Windows version less than 10. See `context_switch.h` file for reasons!
				SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

				// Register graphical window class
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

				// Get graphic system info
				{
					DEVMODEW devmodew = {};
					if(EnumDisplaySettingsW(0, ENUM_CURRENT_SETTINGS, &devmodew)) {
						w32_gfx_state->gfx_info.monitor_refresh_rate = static_cast<f32>(devmodew.dmDisplayFrequency);
					}
				}

				// VKey to OS_Key table
				{
					// Letters
					w32_gfx_state->key_from_vkey_table[(unsigned int)'A'] = Key_A;
					w32_gfx_state->key_from_vkey_table[(unsigned int)'B'] = Key_B;
					w32_gfx_state->key_from_vkey_table[(unsigned int)'C'] = Key_C;
					w32_gfx_state->key_from_vkey_table[(unsigned int)'D'] = Key_D;
					w32_gfx_state->key_from_vkey_table[(unsigned int)'E'] = Key_E;
					w32_gfx_state->key_from_vkey_table[(unsigned int)'F'] = Key_F;
					w32_gfx_state->key_from_vkey_table[(unsigned int)'G'] = Key_G;
					w32_gfx_state->key_from_vkey_table[(unsigned int)'H'] = Key_H;
					w32_gfx_state->key_from_vkey_table[(unsigned int)'I'] = Key_I;
					w32_gfx_state->key_from_vkey_table[(unsigned int)'J'] = Key_J;
					w32_gfx_state->key_from_vkey_table[(unsigned int)'K'] = Key_K;
					w32_gfx_state->key_from_vkey_table[(unsigned int)'L'] = Key_L;
					w32_gfx_state->key_from_vkey_table[(unsigned int)'M'] = Key_M;
					w32_gfx_state->key_from_vkey_table[(unsigned int)'N'] = Key_N;
					w32_gfx_state->key_from_vkey_table[(unsigned int)'O'] = Key_O;
					w32_gfx_state->key_from_vkey_table[(unsigned int)'P'] = Key_P;
					w32_gfx_state->key_from_vkey_table[(unsigned int)'Q'] = Key_Q;
					w32_gfx_state->key_from_vkey_table[(unsigned int)'R'] = Key_R;
					w32_gfx_state->key_from_vkey_table[(unsigned int)'S'] = Key_S;
					w32_gfx_state->key_from_vkey_table[(unsigned int)'T'] = Key_T;
					w32_gfx_state->key_from_vkey_table[(unsigned int)'U'] = Key_U;
					w32_gfx_state->key_from_vkey_table[(unsigned int)'V'] = Key_V;
					w32_gfx_state->key_from_vkey_table[(unsigned int)'W'] = Key_W;
					w32_gfx_state->key_from_vkey_table[(unsigned int)'X'] = Key_X;
					w32_gfx_state->key_from_vkey_table[(unsigned int)'Y'] = Key_Y;
					w32_gfx_state->key_from_vkey_table[(unsigned int)'Z'] = Key_Z;

					// Numbers
					for (u64 i = '0', j = Key_0; i <= '9'; i += 1, j += 1) {
						w32_gfx_state->key_from_vkey_table[i] = (Key)j;
					}

					// Numbers
					for (u64 i = VK_NUMPAD0, j = Key_0; i <= VK_NUMPAD9; i += 1, j += 1) {
						w32_gfx_state->key_from_vkey_table[i] = (Key)j;
					}

					// Function keys: F1-F24
					for (u64 i = VK_F1, j = Key_F1; i <= VK_F24; i += 1, j += 1) {
						w32_gfx_state->key_from_vkey_table[i] = (Key)j;
					}

					// Special keys
					w32_gfx_state->key_from_vkey_table[VK_RETURN] 	= Key_Return;
					w32_gfx_state->key_from_vkey_table[VK_ESCAPE] 	= Key_Escape;
					w32_gfx_state->key_from_vkey_table[VK_BACK] 		= Key_Backspace;
					w32_gfx_state->key_from_vkey_table[VK_TAB] 			= Key_Tab;
					w32_gfx_state->key_from_vkey_table[VK_SPACE]	 	= Key_Space;
					w32_gfx_state->key_from_vkey_table[VK_CAPITAL] 	= Key_CapsLock;

					// Modifiers
					// TODO: differentiate between Left modifiers and right modifiers
					w32_gfx_state->key_from_vkey_table[VK_SHIFT] 		= Key_Shift;
					w32_gfx_state->key_from_vkey_table[VK_RSHIFT] 	= Key_Shift;
					w32_gfx_state->key_from_vkey_table[VK_LSHIFT] 	= Key_Shift;
					w32_gfx_state->key_from_vkey_table[VK_CONTROL] 	= Key_Ctrl;
					w32_gfx_state->key_from_vkey_table[VK_RCONTROL] = Key_Ctrl;
					w32_gfx_state->key_from_vkey_table[VK_LCONTROL] = Key_Ctrl;
					w32_gfx_state->key_from_vkey_table[VK_MENU] 		= Key_Alt;
					w32_gfx_state->key_from_vkey_table[VK_RMENU] 		= Key_Alt;
					w32_gfx_state->key_from_vkey_table[VK_LMENU] 		= Key_Alt;
					w32_gfx_state->key_from_vkey_table[VK_RWIN] 		= Key_Win;
					w32_gfx_state->key_from_vkey_table[VK_LWIN] 		= Key_Win;

					// Arrow keys
					w32_gfx_state->key_from_vkey_table[VK_LEFT] 		= Key_Left;
					w32_gfx_state->key_from_vkey_table[VK_RIGHT] 		= Key_Right;
					w32_gfx_state->key_from_vkey_table[VK_UP] 			= Key_Up;
					w32_gfx_state->key_from_vkey_table[VK_DOWN] 		= Key_Down;

					// Navigation cluster
					w32_gfx_state->key_from_vkey_table[VK_INSERT] 	= Key_Insert;
					w32_gfx_state->key_from_vkey_table[VK_DELETE] 	= Key_Delete;
					w32_gfx_state->key_from_vkey_table[VK_HOME] 		= Key_Home;
					w32_gfx_state->key_from_vkey_table[VK_END] 			= Key_End;
					w32_gfx_state->key_from_vkey_table[VK_PRIOR] 		= Key_PageUp;
					w32_gfx_state->key_from_vkey_table[VK_NEXT] 		= Key_PageDown;

					// Numpad
					w32_gfx_state->key_from_vkey_table[VK_NUMLOCK] 	= Key_NumLock;
					w32_gfx_state->key_from_vkey_table[VK_DIVIDE] 	= Key_NumpadDivide;
					w32_gfx_state->key_from_vkey_table[VK_MULTIPLY] = Key_NumpadMultiply;
					w32_gfx_state->key_from_vkey_table[VK_SUBTRACT] = Key_NumpadMinus;
					w32_gfx_state->key_from_vkey_table[VK_ADD] 			= Key_NumpadPlus;
					w32_gfx_state->key_from_vkey_table[VK_DECIMAL] 	= Key_NumpadDecimal;

					// Punctuation/Symbols
					w32_gfx_state->key_from_vkey_table[VK_OEM_MINUS] = Key_Minus;
					w32_gfx_state->key_from_vkey_table[VK_OEM_PLUS]  = Key_Equals;
					w32_gfx_state->key_from_vkey_table[VK_OEM_1] 		 = Key_Semicolon;
					w32_gfx_state->key_from_vkey_table[VK_OEM_2] 		 = Key_Slash;
					w32_gfx_state->key_from_vkey_table[VK_OEM_3] 		 = Key_Tick;
					w32_gfx_state->key_from_vkey_table[VK_OEM_4] 		 = Key_LeftBracket;
					w32_gfx_state->key_from_vkey_table[VK_OEM_5] 		 = Key_Backslash;
					w32_gfx_state->key_from_vkey_table[VK_OEM_6] 		 = Key_RightBracket;
					w32_gfx_state->key_from_vkey_table[VK_OEM_7] 		 = Key_Quote;
					w32_gfx_state->key_from_vkey_table[VK_OEM_COMMA] = Key_Comma;
					w32_gfx_state->key_from_vkey_table[VK_OEM_PERIOD]= Key_Period;

					// Numpad Numbers
					for (u32 i = 0; i < 10; i += 1) {
						w32_gfx_state->key_from_vkey_table[VK_NUMPAD0 + i] = (Key)((u64)(Key_Numpad0 + i));
					}
				}
			}

			// Graphics system info (Implemented per-os)
			GfxInfo *get_gfx_info(void) { return &w32_gfx_state->gfx_info; }
		}
	}
}