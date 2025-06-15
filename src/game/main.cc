#define BUILD_TITLE                        "Manic"
#define BUILD_VERSION_MAJOR                00
#define BUILD_VERSION_MINOR                00
#define BUILD_VERSION_PATCH                BUILD_SVN_REVISION
#define BUILD_RELEASE_PHASE_STRING_LITERAL "Pre-Alpha"

// Includes, order is important here.
#include "foundation/context_switch.h"
#include "third_party/third_party.h"

// [.h]
#include "foundation/foundation.h"
#include "platform/platform.h"
#include "render/render.h"
#include "draw/draw.h"
#include "resource_manager/resource_manager.h"
#include "game/manic.h"

// [.c]
#include "foundation/foundation.cc"
#include "platform/platform.cc"
#include "render/render.cc"
#include "draw/draw.cc"
#include "resource_manager/resource_manager.cc"

using namespace Starlight::Platform;
using namespace Starlight::Platform::Gfx;

using namespace Starlight::Render;
using namespace Starlight::Draw;


namespace Starlight {
	global GameState* g_window_state = {};
	global b32 quit = 0;

	internal void process_input() {
		ProfFunction();
		Temp scratch = scratch_begin(0, 0);

		EventList events = get_events(scratch.arena, 0);
		for(Event *event = events.first; event != 0; event = event->next) {
			switch(event->event_kind) {
				case(EventKind_WindowClose): {
					quit = 1;
				} break;

				case (EventKind_Press): {
					switch(event->key) {
						case(Key_Q): {
							quit = 1;
						} break;

						default:
							break;
					} break;
				}

				default:
					break;
			}
		}

		scratch_end(scratch);
	}

	internal void initialize_system() {
		ProfFunction();

		// Initialize window and paint into it
		ProfBlock("init window, paint to it", 0);
		{
			Arena* arena = arena_alloc();
			g_window_state = arena_push<GameState>(arena, 1);
			g_window_state->per_frame_memory = arena;
			g_window_state->os_handle = window_open(Rng2f32(0, 0, 1920, 1080), str8_lit(BUILD_TITLE_STRING_LITERAL));
			g_window_state->window_dim = client_rect_from_window(g_window_state->os_handle);
			window_first_paint(g_window_state->os_handle);
			allocate_backbuffer(g_window_state->per_frame_memory, &g_window_state->render_buffer, g_window_state->window_dim.x1, g_window_state->window_dim.y1);
		}

		// Initialize resoure manager
		ProfBlock("init resource manager", 0);
		{
			
		}
		
	}

	

	internal void update() {
		ProfFunction();
		arena_clear(g_window_state->per_frame_memory);

		// @TODO: Handle resize so new space is painted on the next frame.
		//ProfBlock("Resize Window", profDebug_yell); 
		//{
		//	Rng2f32 latest_dim = client_rect_from_window(g_window_state->os_handle);
		//	if(latest_dim.x1 != g_window_state->window_dim.x1 || latest_dim.y1 != g_window_state->window_dim.y1) {
		//		g_window_state->window_dim = latest_dim;
		//		Arena* arena = arena_alloc();
		//		g_window_state = arena_push<GameState>(arena, 1);
		//		g_window_state->per_frame_memory = arena;
		//		allocate_backbuffer(g_window_state->per_frame_memory, &g_window_state->render_buffer, g_window_state->window_dim.x1, g_window_state->window_dim.y1);
		//	}
		//}
	}

	internal void render() {
		ProfFunction();
		clear_colour_buffer(&g_window_state->render_buffer, 0xFF420420);

		draw_grid(&g_window_state->render_buffer, g_window_state->window_dim.x1, g_window_state->window_dim.x1, 0xFFFFFFFF);

		copy_buffer_to_window(g_window_state->os_handle, &g_window_state->render_buffer);
	}

	internal void main_loop() {
		initialize_system();

		while(!quit) {
			process_input();
			update();
			render();

			sleep(16);
		}
	}


}
