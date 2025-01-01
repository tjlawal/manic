// Copyright Frost Gorilla, Inc. All Rights Reserved.

// Threading context functions
C_LINK thread_static ThreadContext *thread_context_thread_local;
#if !BUILD_SUPPLEMENTARY_UNIT
C_LINK thread_static ThreadContext *thread_context_thread_local = 0;
#endif

internal void thread_context_init_and_equip(ThreadContext *thread_context) {
  memory_zero_struct(thread_context);
  Arena **arena_ptr = thread_context->arenas;
  for (u64 i = 0; i < array_count(thread_context->arenas); i += 1, arena_ptr += 1) {
    *arena_ptr = arena_alloc();
  }
  thread_context_thread_local = thread_context;
}

internal void thread_context_release(void) {
  for (u64 i = 0; i < array_count(thread_context_thread_local->arenas); i += 1) {
    arena_release(thread_context_thread_local->arenas[i]);
  }
}

// Return the currently equipped thread in this context
internal ThreadContext *thread_context_get_equipped(void) { return (thread_context_thread_local); }

// NOTE(tijani): This is looking through a collection of arenas to find one that is not
// in conflict and then return it to be used.
internal Arena *thread_context_get_scratch(Arena **conflicts, u64 count) {
  ThreadContext *thread_context = thread_context_get_equipped();

  Arena *result = 0;
  Arena **arena_ptr = thread_context->arenas;
  for (u64 i = 0; i < array_count(thread_context->arenas); i += 1, arena_ptr += 1) {
    Arena **conflicts_ptr = conflicts;
    b32 has_conflict = 0;
    for (u64 j = 0; j < count; j += 1, conflicts_ptr += 1) {
      if (*arena_ptr == *conflicts_ptr) {
        has_conflict = 1;
        break;
      }
    }

    // NOTE(tijani): if has_conflict is not true (i.e. still zero = false), use the arena.
    if (!has_conflict) {
      result = *arena_ptr;
      break;
    }
  }
  return (result);
}

internal void thread_context_set_thread_name(string8 name) {
  ThreadContext *thread_context = thread_context_get_equipped();
  u64 size = clamp_top(name.size, sizeof(thread_context->thread_name));
  memory_copy(thread_context->thread_name, name.str, size);
  thread_context->thread_name_size = size;
}

internal string8 thread_context_get_thread_name(void) {
  ThreadContext *thread_context = thread_context_get_equipped();
  string8 result = str8(thread_context->thread_name, thread_context->thread_name_size);
  return (result);
}

// Thread names
internal void set_thread_name(string8 name) {
  // TODO(tijani): send thread name to profiler
  os_set_thread_name(name);
}

internal void set_thread_namef(char *format, ...) {
  Temp scratch = scratch_begin(0, 0);

  va_list args;
  va_start(args, format);
  string8 string = push_str8fv(scratch.arena, format, args);
  set_thread_name(string);
  va_end(args);

  scratch_end(scratch);
}

internal void thread_context_write_srcloc(char *file_name, u64 line_number) {
  ThreadContext *thread_context = thread_context_get_equipped();
  thread_context->file_name = file_name;
  thread_context->line_number = line_number;
}

internal void thread_context_read_srcloc(char **file_name, u64 *line_number) {
  ThreadContext *thread_context = thread_context_get_equipped();
  *file_name = thread_context->file_name;
  *line_number = thread_context->line_number;
}
