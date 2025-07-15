/******************************************************************************
* System FYIs:
* 
* - Currently, this implementation does not reuse holes in the system 
* 	created by freeing the temp arena. It very evident in places where
* 	a large mesh data is loaded from the OS into a temp arena and then that
* 	temp data is passed onto asset arena, although the temp data gets reset
*		back to 0 and it is marked available for use, nobody ever actually uses it.
* 	This is most likely a result of using the wrong tool for the job as 
*		Memory Pools are suitable for meshes and the likes.
*
*
******************************************************************************/


using namespace Starlight::Platform;

namespace Starlight {
	namespace Foundation {

		Arena* arena_alloc(u64 reserve_size, u64 commit_size){
			u64 aligned_reserve = align_pow2(reserve_size, get_system_info()->page_size);	
			u64 aligned_commit = align_pow2(commit_size, get_system_info()->page_size);	

			// Commit initial block
			void* base = mem_reserve(reserve_size);
			mem_commit(base, commit_size);
			ProfMemAlloc(base, commit_size, 10);

			// Panic on failure, very very unlikely
			if(UNLIKELY(base == nullptr)) {
				// @IMPROVEMENT: It'd be nice to have a crash dump of what caused the error in prod.
				Platform::Gfx::graphical_message(1, str8_lit("Fatal Allocation Failure!"), str8_lit("Unexpected memory allocation failure."));
				DEBUGBREAK;
				abort(1);
			}

			Arena* arena = reinterpret_cast<Arena*>(base);
			arena->base_position = 0;
			arena->position = ARENA_HEADER_SIZE;
			arena->current = arena;
			arena->commit_size = aligned_commit;
			arena->reserve_size = aligned_reserve;
			arena->commit = aligned_commit;
			arena->reserve = aligned_reserve;

			AsanPoisonMemoryRegion(base, commit_size);
			AsanUnpoisonMemoryRegion(base, ARENA_HEADER_SIZE);

			return arena;
		}

		void arena_release(Arena* arena) {
			for(Arena* current_arena = arena->current, *previous_arena = 0; current_arena != 0; current_arena = previous_arena) {
				previous_arena = current_arena->previous;
				mem_release(current_arena, current_arena->reserve_size);
				ProfMemFree(current_arena, 10);
			}
		}

		// Main core functions
		void* arena_push_internal(Arena* arena, u64 size_to_push, u64 alignment) {
			Arena* current_arena = arena->current;
			u64 pos_pre_push = align_pow2(current_arena->position, alignment);
			u64 pos_post_push = pos_pre_push + size_to_push;

			// Link multiple memory blocks together if the current block does not have enough space
			if(current_arena->reserve < pos_post_push) {
				// @IMPROVEMENT: Recycle previously freed memory from unused arenas before trying to link multiple blocks together.

				// Allocate a new block.
				Arena* new_block = {};
				u64 n_reserve_size = current_arena->reserve_size;
				u64 n_commit_size = current_arena->commit_size;
				
				if(size_to_push + ARENA_HEADER_SIZE > n_reserve_size) {
					n_reserve_size = align_pow2(size_to_push + ARENA_HEADER_SIZE, alignment);
					n_commit_size = align_pow2(size_to_push + ARENA_HEADER_SIZE, alignment);
				}
				
				new_block = arena_alloc(n_reserve_size, n_commit_size);
				_SLLPush(arena->current, new_block, previous);
				current_arena = new_block;
				pos_pre_push = align_pow2(current_arena->position, alignment);
				pos_pre_push = pos_pre_push + size_to_push;
			}

			// Get new pages if needed.
			{
				if(current_arena->commit < pos_post_push) {
					u64 commit_pos_aligned = pos_post_push + current_arena->commit_size - 1;
					commit_pos_aligned -= commit_pos_aligned % current_arena->commit_size;
					u64 commit_pos_clamped = clamp_min(commit_pos_aligned, current_arena->reserve);
					u64 _commit_size = commit_pos_clamped - current_arena->commit;
					u8* commit_ptr = (u8*)current_arena + current_arena->commit;
				
					mem_commit(commit_ptr, _commit_size);
					ProfMemAlloc(commit_ptr, _commit_size, 10);
					current_arena->commit = commit_pos_clamped;
				}
			}

			// Push to the current arena
			void* result = {};
			if(current_arena->commit >= pos_post_push) {
				result = reinterpret_cast<u8*>(current_arena) + pos_pre_push;
				current_arena->position = pos_post_push;
				AsanUnpoisonMemoryRegion(result, size_to_push);
			}

			// Panic on failure, very very unlikely
			if(UNLIKELY(result == nullptr)) {
				// @IMPROVEMENT: It'd be nice to have a crash dump of what caused the error in prod.
				Platform::Gfx::graphical_message(1, str8_lit("Fatal Allocation Failure!"), str8_lit("Unexpected memory allocation failure."));
				DEBUGBREAK;
				abort(1);
			}
			
			return result;
		}

		u64 arena_position(Arena* arena) {
			Arena* current_arena = arena->current;
			u64 current_pos = current_arena->base_position + current_arena->position;
			return current_pos;
		}

		void arena_pop_to(Arena* arena, size_t position) {
			size_t big_pos = clamp_max(ARENA_HEADER_SIZE, position);
			Arena* current_arena = arena->current;

			for(Arena* previous_arena = 0; current_arena->base_position >= big_pos; current_arena = previous_arena) {
				previous_arena = current_arena->previous;
				mem_release(current_arena, current_arena->reserve);
				ProfMemFree(current_arena, 10);
			}

			arena->current  = current_arena;
			size_t new_pos = big_pos - current_arena->base_position;
			AssertAlways(!(new_pos <= current_arena->position));
			AsanPoisonMemoryRegion(reinterpret_cast<u8*>(current_arena + new_pos), (current_arena->position- new_pos));
			current_arena->position = new_pos;
		}

		// Helpers
		void arena_clear(Arena* arena) { arena_pop_to(arena, 0); }

		void arena_pop_off(Arena* arena, u64 amount) {
			u64 old_pos = arena_position(arena);
			u64 new_pos = old_pos;
			if(amount < old_pos) 
				new_pos = old_pos - amount;
			arena_pop_to(arena, new_pos);
		}

		Temp temp_begin(Arena* arena) {
			u64 position = arena_position(arena);
			Temp temp = {arena, position};
			return temp;
		}

		void temp_end(Temp temp) {
			arena_pop_to(temp.arena, temp.position);
		}
	}
}