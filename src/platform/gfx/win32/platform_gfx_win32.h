#pragma once

using namespace Starlight::Foundation;

namespace Starlight {
	namespace Platform {
		namespace Gfx {
			
			struct Window {
				Window* next;
				Window* previous;
				HWND hwnd;
				HDC hdc;
				WINDOWPLACEMENT window_placement;
				f32 dpi;
				b32 first_paint;
				b32 maximized;
			};

			struct GfxState {
				Key key_from_vkey_table[256];
				Arena* arena;
				Window* first_window;
				Window* last_window;
				Window* free_window;
				HINSTANCE hInstance;
				HCURSOR hCursor;
				GfxInfo gfx_info;
				u32 gfx_thread_id;
			};

			// Globals
			global Arena* w32_event_arena = nullptr;
			global GfxState* w32_gfx_state = nullptr;
			global EventList w32_event_list = {};

			// Win32 Specifics
			internal Window* w32_window_alloc(void);
			internal LRESULT w32_window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
			internal void 	 w32_window_release(Window *window);
			internal Event*  w32_push_event(EventKind kind, Window *window);
			internal Key 		 key_from_w32_vkey(WPARAM virtual_key);
			internal WPARAM  w32_vkey_from_key(Key key);

			// WIN32 to/from platform layer windowing functions
			internal Handle  w32_handle_from_window(Window *window);
			internal Window* w32_window_from_handle(Handle window);
			internal HWND 	 w32_hwnd_from_window(Window *window);
			internal Window* w32_window_from_hwnd(HWND hwnd);
			internal Rng2f32 w32_rng_from_rect(RECT rect);
		}
	}
}