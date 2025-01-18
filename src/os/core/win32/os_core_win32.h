// Copyright Frost Gorilla, Inc. All Rights Reserved.

#ifndef OS_CORE_WIN32_H
#define OS_CORE_WIN32_H

#define WIN32_LEAN_AND_MEAN
// clang-format off

#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <processthreadsapi.h>

// clang-format on

#pragma comment(lib, "user32")
#pragma comment(lib, "shell32")
#pragma comment(lib, "gdi32")
#pragma comment(lib, "shlwapi")
#pragma comment(lib, "comctl32")

// NOTE(tijani): This line is required for loading the correct comctl32 dll
// file. It ensures the correct version (6) is selected to enable visual styles
// for the exception window.
#pragma comment(                                                                                                       \
    linker,                                                                                                            \
    "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// Windows State
typedef struct OS_W32_State OS_W32_State;
struct OS_W32_State {
  Arena *arena;
  OS_SystemInfo system_info;
  OS_ProcessInfo process_info;
  u64 microsecond_resolution;
};

// NOTE(tijani): Win32 Globals
global OS_W32_State os_w32_state = {0};

// Thread entry point
internal DWORD os_w32_thread_entry_point(void *ptr);

#endif // OS_CORE_WIN32_H