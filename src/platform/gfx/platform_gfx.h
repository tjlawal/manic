#pragma once

#include "platform_gfx.inl"

using namespace Starlight::Foundation;

namespace Starlight {
	namespace Platform {
		namespace Gfx {

			// Graphical system information
			struct GfxInfo {
				f32 monitor_refresh_rate;
				//f32 double_click_time;
				//f32 caret_blink_time;
			};

			struct Event {
				Event *next;
				Event *previous;
				EventKind event_kind;
				Handle window;

				// Keyboard event data
				KeyModifiers key_modifiers;
				Key key;
				b32 is_repeat;
				b32 right_sided;
				b32 left_sided;
				b32 character;
				u32 repeat_count;
				u64 timestamp_us;

				// Mouse event data
				Vec2f position;
				Vec2f delta;
			};

			typedef struct EventList EventList;
			struct EventList {
				u64 count;
				Event *first;
				Event *last;
			};

			// Main initialization
			internal void gfx_init(void);

			// Graphics system info
			internal GfxInfo* get_gfx_info(void);

			// Windowing function
			internal Handle  	window_open(Rng2f32 window_size, string8 title);
			internal void     window_close(Handle window_handle);
			internal void    	window_first_paint(Handle window_handle);
			internal void 	 	window_set_fullscreen(Handle window_handle);
			internal Rng2f32  client_rect_from_window(Handle window_handle);

			// TODO: Delete me
			internal Vec2s 		get_window_dimension(Handle handle);
			internal void*	 	get_device_context(Handle handle);
			internal void 	 	release_device_context(Handle handle, void *device_context);

			// OS Events
			internal Event* 			event_list_push_new(Arena* arena, EventList *events, EventKind event_kind);
			internal EventList 		get_events(Arena* arena, b32 wait);
			internal KeyModifiers get_modifiers(void);

			// Native facing graphical message (implemented per-os)
			internal void	graphical_message(b32 error, string8 title, string8 message);

		}
	}
}