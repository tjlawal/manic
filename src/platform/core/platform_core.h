#pragma once

// Platform agnostic layer code

using namespace Starlight::Foundation;

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

		struct FileProperty {
			u64 size;
			u64 modified;
			u64 created;
			FilePropertyFlag flags;
		};

		// OS agnostic handles
		internal Handle handle_zero(void);
		internal b32 handle_match(Handle a, Handle b);

		// 
		internal SystemInfo* get_system_info(void);
		internal ProcessInfo* get_process_info(void);

		// Memory allocation
		internal void* mem_reserve(u64 size);
		internal b32 	 mem_commit(void *ptr, u64 size);
		internal void  mem_decommit(void *ptr, u64 size);
		internal void  mem_release(void *ptr, u64 size);

		// Operating System I/O abstraction primitives
		internal FileProperty properties_from_file(Handle file);
		internal Handle 			open_file(AccessFlags flags, string8 path);
		internal void 				close_file(Handle file);
		internal u64 					read_file(Handle file, Rng1u64 range, void* output_data);
		internal b32 					write_file(Handle file, Rng1u64 range, void* data);

		// File system, these abstract away the system primitives
		internal b32 		 write_data_to_file(string8 file_path, string8 data);
		internal b32 		 append_data_to_file(string8 file_path, string8 data);
		internal string8 read_data_from_file(Arena* arena, string8 file_path);
		internal string8 data_from_file_range(Arena* arena, Handle file, Rng1u64 range);

		// OS halting primitives
		internal void  abort(s32 exit_code);
		internal void  sleep(u64 ns);
	}
}