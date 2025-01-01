// Copyright Frost Gorilla, Inc. All Rights Reserved.

#ifndef OS_CORE_H
#define OS_CORE_H

// Handle Type
typedef struct OS_Handle OS_Handle;
struct OS_Handle {
  u64 handle[1];
};

// tijani: Thread Types

typedef void OS_ThreadFunctionType(void *ptr);

// System information
typedef struct OS_SystemInfo OS_SystemInfo;
struct OS_SystemInfo {
  u32 logical_processor_count;
  u64 page_size;
  u64 large_page_size;
  u64 allocation_granularity;
  string8 machine_name;
};

// Process information
typedef struct OS_ProcessInfo OS_ProcessInfo;
struct OS_ProcessInfo {
  u32 process_id;
  b32 large_pages_allowed;
  string8 binary_path;
  string8 initial_path;
  string8 user_program_data_path;
  string8list module_load_paths;
  string8list environment;
};

// System info (implemented per-os)
internal OS_SystemInfo *os_get_system_info(void);
internal OS_ProcessInfo *os_get_process_info(void);
internal string8 os_get_current_path(Arena *arena);

// Memory allocation (implemented per-os)
internal void *os_reserve(u64 size);
internal b32 os_commit(void *ptr, u64 size);
internal void os_decommit(void *ptr, u64 size);
internal void os_release(void *ptr, u64 size);

internal void *os_reserve_large(u64 size);
internal b32 os_commit_large(void *ptr, u64 size);

// Thread Info (implemented per-os)
internal u32 os_thread_id(void);
internal void os_set_thread_name(string8 name);

// Aborting (implemented per-os)
internal void os_abort(s32 exit_code);

// Application Entry Point (implemented per-os)

// NOTE(tijani): `os_core` defines the low-level entry points if
// BUILD_ENTRY_DEFINING_UNIT is set to 1. this will call into the standard
// codebase program entry points, named "entry_point". This would allow to have
// multiple console programs in the code base that are console oriented but
// still have access to all the aspects of the codebase. Credit to Ryan Fluery
// of Epic Games Tools Raddebugger.

#if BUILD_ENTRY_DEFINING_UNIT
internal void entry_point();
#endif

#endif // OS_CORE_H