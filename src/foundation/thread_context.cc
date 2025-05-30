
using namespace Starlight::Foundation;

namespace Starlight {
	namespace Foundation {
		thread_local ThreadContext* tctxt_thread_local = 0;

		void tctxt_init_and_engage(ThreadContext* tctxt) {
			MemoryZeroStruct(tctxt);
			Arena** arena_ptr = tctxt->arenas;

			for(u64 i = 0; i < ArrayCount(tctxt->arenas); i += 1, arena_ptr += 1) {
				*arena_ptr = arena_alloc();
			}
			tctxt_thread_local = tctxt;
		}

		void tctxt_release(void) {
			for(u64 i = 0; i < ArrayCount(tctxt_thread_local->arenas); i += 1) {
				arena_release(tctxt_thread_local->arenas[i]);
			}
		}

		ThreadContext* tctxt_get_equipped(void) { return tctxt_thread_local; }
		
		Arena* tctxt_get_scratch_memory(Arena** conflicts, u64 count) {
			ThreadContext *ctxt = tctxt_get_equipped();
			Arena* result = {};
			Arena** arena_ptr = ctxt->arenas;

			for(u64 i = 0; i < ArrayCount(ctxt->arenas); i += 1, arena_ptr += 1) {
				Arena** conflicts_ptr = conflicts;
				b32 conflict = 0;

				for(u64 j = 0; j < count; j += 1, conflicts_ptr += 1) {
					if(*arena_ptr == *conflicts_ptr) {
						conflict = 1;
						break;
					}
				}

				// No conflicts, arena is clear to use.
				if(!conflicts) {
					result = *arena_ptr;
					break;
				}
			}

			return result;
		}
		
		//void tctxt_set_thread_name(string8 name) {}
		//string8 tctxt_get_thread_name(void) {}

	}
}

