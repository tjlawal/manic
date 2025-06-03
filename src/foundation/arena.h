#pragma once

#define ARENA_HEADER_SIZE 128
//const u8 ARENA_HEADER_SIZE = 128;

namespace Starlight {
	namespace Foundation {

		global u64 g_default_reserve_size = MB(512); // Maximum memory that would be used throughout its entire lifetime.
		global u64 g_default_commit_size = KB(64);   // Size of each arena block allocated.

		// Chained arena allocator
		struct Arena {
			Arena* previous;
			Arena* current;
			u64 base_position;
			u64 position;
			u64 commit;
			u64 reserve;
			u64 reserve_size;
			u64 commit_size;
		};

		// Core functions
		internal Arena* arena_alloc(u64 reserve_size = g_default_reserve_size, u64 commit_size = g_default_commit_size);
		internal void*  arena_push_internal(Arena* arena, u64 size_to_push, u64 alignment);
		internal void   arena_release(Arena* arena);
		internal u64    arena_position(Arena* arena);
		internal void   arena_pop_to(Arena* arena, u64 position);
		
		// Helpers
		internal void  arena_pop_off(Arena* arena, u64 amount);
		internal void  arena_clear(Arena* arena);

		// Arena is not zeroed out but aligned to 8-byte boundary.
		template<typename T>
		internal T* arena_push_non_zeroed_aligned(Arena* arena, u64 size) {
			ProfBlock(0, profDebug_red);

			return reinterpret_cast<T*>(arena_push_internal(arena, size, max(8, alignof(T))));
		}

		// Arena is zeroed out and aligned to a .
		template<typename T>
		internal T* arena_push(Arena* arena, u64 size) {
			ProfBlock(0, profDebug_red);

			return reinterpret_cast<T*>(MemoryZero(arena_push_internal(arena, size, max(8, alignof(T))), sizeof(T)*size));
		}

		struct Temp{
			Arena* arena;
			u64 position;
		};

		internal Temp temp_begin(Arena* arena);
		internal void temp_end(Temp temp);
	}

}