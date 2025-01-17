// Copyright Frost Gorilla, Inc. All Rights Reserved.

internal Arena *arena_alloc_(ArenaParams *params) {
  u64 reserve_size = params->reserve_size;
  u64 commit_size = params->commit_size;

  if (params->flags & ArenaFlag_LargePages) {
    reserve_size = align_pow2(reserve_size, os_get_system_info()->large_page_size);
    commit_size = align_pow2(commit_size, os_get_system_info()->large_page_size);
  } else {
    reserve_size = align_pow2(reserve_size, os_get_system_info()->page_size);
    commit_size = align_pow2(commit_size, os_get_system_info()->page_size);
  }

  // reserve/commit initial block of memory
  void *base = params->optional_preallocated_buffer;
  if (base == 0) {
    if (params->flags & ArenaFlag_LargePages) {
      base = os_reserve_large(reserve_size);
      os_commit_large(base, commit_size);
    } else {
      base = os_reserve(reserve_size);
      os_commit(base, commit_size);
    }
  }

// panic on failure
#if OS_FEATURE_GRAPHICAL
  if (unlikely(base == 0)) {
    os_graphical_message(1, str8_lit("Fatal Allocation Failure"), str8_lit("Unexpected memory allocation failure."));
    os_abort(1);
  }
#endif

  Arena *arena = (Arena *)base;
  arena->current = arena; // Current arena in the chain
  arena->flags = params->flags;
  arena->commit_size = params->commit_size;
  arena->reserve_size = params->reserve_size;
  arena->base_position = 0;
  arena->position = ARENA_HEADER_SIZE;
  arena->commit = commit_size;
  arena->reserve = reserve_size;
#if ARENA_FREE_LIST
  arena->free_size = 0;
  arena->free_last = 0;
#endif
  AsanPoisonMemoryRegion(base, commit_size);
  AsanUnpoisonMemoryRegion(base, ARENA_HEADER_SIZE);
  return arena;
}

internal void arena_release(Arena *arena) {
  for (Arena *current_arena = arena->current, *previous_arena = 0; current_arena != 0; current_arena = previous_arena) {
    previous_arena = current_arena->previous;
    os_release(current_arena, current_arena->reserve);
  }
}

internal void *arena_push(Arena *arena, u64 size_to_push, u64 alignment) {
  Arena *current_arena = arena->current;
  u64 position_pre_push = align_pow2(current_arena->position, alignment); // position of arena before push
  u64 position_post_push = position_pre_push + size_to_push;              // position of arena after push

  // NOTE(tijani): link multiple arenas memory blocks together if the current
  // memory block does not have enough space
  if (current_arena->reserve < position_post_push && !(arena->flags & ArenaFlag_NoChain)) {
    Arena *new_block = 0;

// NOTE(tijani): if available, use previously freed memory from the arena that
// is currently not being used instead of allocating a new block
#if ARENA_FREE_LIST
    Arena *previous_block;
    for (new_block = arena->free_last, previous_block = 0; new_block != 0;
         previous_block = new_block, new_block = new_block->previous) {
      if (new_block->reserve >= align_pow2(size_to_push, alignment)) {
        if (previous_block) {
          previous_block->previous = new_block->previous;
        } else {
          arena->free_last = new_block->previous;
        }
        arena->free_size -= new_block->reserve_size;
        AsanUnpoisonMemoryRegion((u8 *)new_block + ARENA_HEADER_SIZE, new_block->reserve_size - ARENA_HEADER_SIZE);
        break;
      }
    }
#endif

    if (new_block == 0) {
      u64 reserve_size = current_arena->reserve_size;
      u64 commit_size = current_arena->commit_size;
      if (size_to_push + ARENA_HEADER_SIZE > reserve_size) {
        reserve_size = align_pow2(size_to_push + ARENA_HEADER_SIZE, alignment);
        commit_size = align_pow2(size_to_push + ARENA_HEADER_SIZE, alignment);
      }
      new_block = arena_alloc(.reserve_size = reserve_size, .commit_size = commit_size, .flags = current_arena->flags);
    }
    new_block->base_position = current_arena->base_position + current_arena->reserve;
    SLL_StackPushNext(arena->current, new_block, previous);
    current_arena = new_block;
    position_pre_push = align_pow2(current_arena->position, alignment);
    position_post_push = position_pre_push + size_to_push;
  }

  // Commit new pages if needed
  if (current_arena->commit < position_post_push) {
    u64 commit_position_aligned = position_post_push + current_arena->commit_size - 1;
    commit_position_aligned -= commit_position_aligned % current_arena->commit_size;
    u64 commit_position_clamped = CLAMP_TOP(commit_position_aligned, current_arena->reserve);
    u64 commit_size = commit_position_clamped - current_arena->commit;
    u8 *commit_ptr = (u8 *)current_arena + current_arena->commit;

    if (current_arena->flags & ArenaFlag_LargePages) {
      os_commit_large(commit_ptr, commit_size);
    } else {
      os_commit(commit_ptr, commit_size);
    }
    current_arena->commit = commit_position_clamped;
  }

  // push onto current block
  void *result = 0;
  if (current_arena->commit >= position_post_push) {
    result = (u8 *)current_arena + position_pre_push;
    current_arena->position = position_post_push;
    AsanUnpoisonMemoryRegion(result, size_to_push);
  }

// panic when fail
#if OS_FEATURE_GRAPHICAL
  if (unlikely(result == 0)) {
    os_graphical_message(1, str8_lit("Fatal allocation failure"), str8_lit("Unexpected memory allocation failure"));
    os_abort(1);
  }
#endif
  return result;
}

internal u64 arena_position(Arena *arena) {
  Arena *current_arena = arena->current;
  u64 current_position = current_arena->base_position + current_arena->position;
  return current_position;
}

internal void arena_pop_to(Arena *arena, u64 position) {
  u64 big_position = CLAMP_BOT(ARENA_HEADER_SIZE, position);
  Arena *current_arena = arena->current;

#if ARENA_FREE_LIST
  for (Arena *previous = 0; current_arena->base_position >= big_position; current_arena = previous) {
    previous = current_arena->previous;
    current_arena->position = ARENA_HEADER_SIZE;
    arena->free_size += current_arena->reserve_size;
    SLL_StackPushNext(arena->free_last, current_arena, previous);
    AsanPoisonMemoryRegion((u8 *)current_arena + ARENA_HEADER_SIZE, current_arena->reserve_size - ARENA_HEADER_SIZE);
  }
#else
  for (Arena *previous_arena = 0; current_arena->base_position >= big_position; current_arena = previous_arena) {
    previous_arena = current_arena->previous;
    os_release(current_arena, current_arena->reserve);
  }

#endif
  arena->current = current_arena;
  u64 new_position = big_position - current_arena->base_position;
  assert_always(new_position <= current_arena->position);
  AsanPoisonMemoryRegion((u8 *)current_arena + new_position, (current_arena->position - new_position));
  current_arena->position = new_position;
}

internal void arena_clear(Arena *arena) { arena_pop_to(arena, 0); }

// Pops the amount of memory passed off the passed in arena.
// The new position of the passed in arena will be the same
// memory block minus the amount passed in.
internal void arena_pop_off(Arena *arena, u64 amount) {
  u64 old_position = arena_position(arena);
  u64 new_position = old_position;

  if (amount < old_position) {
    new_position = old_position - amount;
  }

  arena_pop_to(arena, new_position);
}

// Temporary arena scope
internal Temp temp_begin(Arena *arena) {
  u64 position = arena_position(arena);
  Temp temp = {arena, position};
  return temp;
}

internal void temp_end(Temp temp) { arena_pop_to(temp.arena, temp.position); }
