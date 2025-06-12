
// Platform agnostic

namespace Starlight {
	namespace Platform {

		struct SystemInfo {
			u32 logical_processor_count;
			u64 page_size;
			u64 large_page_size;
			u64 allocation_granularity;
		};

		// Process information
		struct ProcessInfo {
			u32 process_id;
			b32 large_pages_allowed;
		};

		// Handle Type
		struct Handle {
			u64 handle[1];
		};

		// OS agnostic handles
		internal Handle handle_zero(void);
		internal b32 handle_match(Handle a, Handle b);

		internal SystemInfo* get_system_info(void);
		internal ProcessInfo* get_process_info(void);

		// Memory allocation
		internal void *mem_reserve(u64 size);
		internal void *mem_reserve_large(u64 size);
		internal b32 	 mem_commit(void *ptr, u64 size);
		internal b32   mem_commit_large(void *ptr, u64 size);
		internal void  mem_decommit(void *ptr, u64 size);
		internal void  mem_release(void *ptr, u64 size);

		internal void  abort(s32 exit_code);
		internal void  sleep(u64 ns);
	}
}