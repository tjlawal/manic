#define BUILD_TITLE                        "Manic"
#define BUILD_VERSION_MAJOR                00
#define BUILD_VERSION_MINOR                00
#define BUILD_VERSION_PATCH                BUILD_SVN_REVISION
#define BUILD_RELEASE_PHASE_STRING_LITERAL "Alpha"

// Includes, order is important here.
#include "foundation/context_switch.h"
#include "third_party/third_party.h"

// [.h]
#include "foundation/foundation.h"
#include "platform/platform.h"
#include "render/render.h"
#include "draw/draw.h"

// [.c]
#include "foundation/foundation.cc"
#include "platform/platform.cc"
#include "render/render.cc"
#include "draw/draw.cc"

using namespace Starlight::Platform;
using namespace Starlight::Platform::Gfx;

using namespace Starlight::Render;
using namespace Starlight::Draw;


namespace Starlight {
	
	global Handle window_handle = {};
	global Vec2S32 window_dimensions = {};
	global HDC device_context = {};
	global Renderer render_buffer = {};
	global b32 quit = 0;


	internal void system_setup() {
		ProfBlock(0, profDebug_cadetblue);

		window_handle = window_open(Vec2S32(1920, 1080), str8_lit(BUILD_TITLE_STRING_LITERAL));
		window_first_paint(window_handle);
		device_context = reinterpret_cast<HDC>(get_device_context(window_handle));
	}

	internal void process_input() {
		ProfBlock(0, profDebug_gold);
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
					}
				}

				default:
					break;
			}
		}

		scratch_end(scratch);
	}

	internal void update() {
		Temp scratch = scratch_begin(0, 0);
		ProfBlock(0, profDebug_hotpink);

		// Handle window resize
		window_dimensions = get_window_dimension(window_handle);
		r_resize_buffer(scratch.arena, &render_buffer, window_dimensions.x, window_dimensions.y);


		scratch_end(scratch);
	}

	internal void render() {
		ProfBlock(0, profDebug_darkblue);
		r_clear_colour_buffer(&render_buffer, 0xFF2C2C2C);

		r_copy_buffer_to_window(device_context, &render_buffer);
	}

	internal void Platform::MainLoop() {
		system_setup();

		while(!quit) {
			process_input();
			update();
			render();

			sleep(300);
		}
	}


}
