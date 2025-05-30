using namespace Starlight::Foundation;

namespace Starlight {
	namespace Platform {
		namespace Gfx {

			internal Event *event_list_push_new(Arena* arena, EventList *events, EventKind event_kind) {
				Event* event = arena_push<Event>(arena, 1);
				DLL_PushBack(events->first, events->last, event);
				events->count += 1;

				event->event_kind = event_kind;
				return event;
			}

		}
	}
}