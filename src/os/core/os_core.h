// Copyright Frost Gorilla, Inc. All Rights Reserved.

// clang-format off
#ifndef OS_CORE_H
#define OS_CORE_H

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
  u32 				process_id;
  b32 				large_pages_allowed;
  string8 		binary_path;
  string8 		initial_path;
  string8 		user_program_data_path;
  string8list module_load_paths;
  string8list environment;
};

// Handle Type
typedef struct OS_Handle OS_Handle;
struct OS_Handle {
  u64 handle[1];
};

// Access flags
typedef u32 OS_AccessFlags;
enum {
  OS_AccessFlag_Read = (1 << 0),
  OS_AccessFlag_Write = (1 << 1),
  OS_AccessFlag_Append = (1 << 2),
};

// Handle Types (implemented per OS)
internal OS_Handle os_handle_zero(void);
internal b32 			 os_handle_match(OS_Handle a, OS_Handle b);

// System info (implemented per-os)
internal OS_SystemInfo  *os_get_system_info(void);
internal OS_ProcessInfo *os_get_process_info(void);
internal string8 				 os_get_current_path(Arena *arena);

// Memory allocation (implemented per-os)
internal void *os_reserve(u64 size);
internal b32 	 os_commit(void *ptr, u64 size);
internal void  os_decommit(void *ptr, u64 size);
internal void  os_release(void *ptr, u64 size);
internal void *os_reserve_large(u64 size);
internal b32   os_commit_large(void *ptr, u64 size);

// Aborting (implemented per-os)
internal void os_abort(s32 exit_code);

// Time (implemented per-os)
//internal u64  os_now_microseconds(void);

// File system (implemented per-os)
internal OS_Handle os_file_open(OS_AccessFlags flags, string8 path);
internal void			 os_file_close(OS_Handle file);
internal u64 			 os_file_read(OS_Handle file, Rng1u64 rng, void *data_dest);
internal u64       os_file_write(OS_Handle file, Rng1u64 rng, void *data);

// Application Entry Point (implemented per-os)

// NOTE(tijani): `os_core` defines the low-level entry points if
// BUILD_ENTRY_DEFINING_UNIT is set to 1. this will call into the standard
// codebase program entry points, named "entry_point". This would allow to have
// multiple console programs in the code base that are console oriented but
// still have access to all the aspects of the codebase. Credit to Epic Games
// Tools Raddebugger.

#if BUILD_ENTRY_DEFINING_UNIT
internal void entry_point();
#endif

#endif // OS_CORE_H