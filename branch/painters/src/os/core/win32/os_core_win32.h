// Copyright Frost Gorilla, Inc. All Rights Reserved.

#ifndef OS_CORE_WIN32_H
#define OS_CORE_WIN32_H

#define WIN32_LEAN_AND_MEAN
// clang-format off

#include <windows.h>
#include <shellapi.h>
#include <processthreadsapi.h>

// clang-format on

#pragma comment(lib, "user32")
#pragma comment(lib, "shell32")
#pragma comment(lib, "gdi32")

// tijani: Thread Primitives
typedef enum OS_W32_ThreadPrimitiveKind {
  OS_W32_ThreadPrimitiveKind_NULL,
  OS_W32_ThreadPrimitiveKind_Thread,
  OS_W32_ThreadPrimitiveKind_Mutex,
  OS_W32_ThreadPrimitiveKind_RWMutex,
  OS_W32_ThreadPrimitiveKind_ConditionVariable,
} OS_W32_ThreadPrimitiveKind;

typedef struct OS_W32_ThreadPrimitive OS_W32_ThreadPrimitive;
struct OS_W32_ThreadPrimitive {
  OS_W32_ThreadPrimitive *next;
  OS_W32_ThreadPrimitiveKind *thread_primitive_kind;
  union {
    struct {
      OS_ThreadFunctionType *func;
      void *ptr;
      HANDLE handle;
      DWORD thread_id;
    } thread;
    CRITICAL_SECTION mutex;
    SRWLOCK read_write_mutex;
    CONDITION_VARIABLE condition_variable;
  };
};

// Windows State
typedef struct OS_W32_State OS_W32_State;
struct OS_W32_State {
  Arena *arena;
  OS_SystemInfo system_info;
  OS_ProcessInfo process_info;
  // TODO(tijani): perfect place to store timing information for windows
  // using QueryPerformanceCounter.
};

// NOTE(tijani): Win32 Globals
global OS_W32_State os_w32_state = {0};

// Thread entry point
internal DWORD os_w32_thread_entry_point(void *ptr);

#endif // OS_CORE_WIN32_H