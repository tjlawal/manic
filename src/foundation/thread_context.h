#pragma once

using namespace Starlight::Foundation;

namespace Starlight {
	namespace Foundation {
		
		struct ThreadContext {
			Arena* arenas[2];
			u8 thread_name[32];
			u64 thread_name_size;


			internal void tctxt_init_and_engage(ThreadContext* tctxt);
			internal void tctxt_release(void);

			internal ThreadContext* tctxt_get_equipped(void);
			internal Arena* tctxt_get_scratch_memory(Arena** conflicts, u64 count);

			internal void tctxt_set_thread_name(string8 name);
			internal string8 tctxt_get_thread_name(void);
		};
	}

}

#define scratch_begin(conflicts, count) temp_begin(tctxt_get_scratch_memory((conflicts), (count)))
#define scratch_end(scratch_memory) 		temp_end(scratch)
