// Copyright Frost Gorilla, Inc. All Rights Reserved.

#ifndef BASE_THREAD_CONTEXT_H
#define BASE_THREAD_CONTEXT_H

// Thread context
typedef struct ThreadContext ThreadContext;
struct ThreadContext {
  Arena *arenas[2];

  u8 thread_name[32];
  u64 thread_name_size;

  // NOTE(tijani): these are for debugging.
  char *file_name;
  u64 line_number;
};

// Thread context functions
internal void thread_context_init_and_equip(ThreadContext *thread_context);
internal void thread_context_release(void);
internal ThreadContext *thread_context_get_equipped(void);

internal Arena *thread_context_get_scratch(Arena **conflicts, u64 count);

// Thread helper functions and macros
internal void thread_context_set_thread_name(string8 name);
internal string8 thread_context_get_thread_name(void);
internal void set_thread_name(string8 name);
internal void set_thread_namef(char *format, ...);
#define ThreadNameF(...) (set_thread_namef(__VA_ARGS__))
#define ThreadName(str)  (set_thread_name(str))

// NOTE(tijani): these are useful for debugging.
internal void thread_context_write_srcloc(char *file_name, u64 line_number);
internal void thread_context_read_srcloc(char **file_name, u64 *line_number);
#define thread_context_write_this_srcloc() thread_context_write_srcloc(__FILE__, __LINE__)

// NOTE(tijani): Per thread arena allocation
#define scratch_begin(conflicts, count) temp_begin(thread_context_get_scratch((conflicts), (count)))
#define scratch_end(scratch)            temp_end(scratch)

#endif // BASE_THREAD_CONTEXT_H