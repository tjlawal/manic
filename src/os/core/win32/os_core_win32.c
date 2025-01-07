// Copyright Frost Gorilla, Inc. All Rights Reserved.

//////////////////////
// Thread Entry Point
internal DWORD os_w32_thread_entry_point(void *ptr) {
  OS_W32_ThreadPrimitive *primitive = (OS_W32_ThreadPrimitive *)ptr;
  OS_ThreadFunctionType *func = primitive->thread.func;
  void *thread_ptr = primitive->thread.ptr;
  ThreadContext thread_context_;
  thread_context_init_and_equip(&thread_context_);
  func(thread_ptr);
  thread_context_release();
  return 0;
}

///////////////////////
// System info (implemented per-os)

internal OS_SystemInfo *os_get_system_info(void) { return &os_w32_state.system_info; }

internal OS_ProcessInfo *os_get_process_info(void) { return &os_w32_state.process_info; }

////////////////////////
// Memory allocation (implemented per-os)

internal void *os_reserve(u64 size) {
  void *result = VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);

  if (result == NULL) {
    DWORD error = GetLastError();
    // Log or debug print the error
    printf("VirtualAlloc failed with error: %lu\n", error);
  }
  return result;
}

internal b32 os_commit(void *ptr, u64 size) {
  b32 result = (VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != 0);
  return result;
}

internal void os_decommit(void *ptr, u64 size) { VirtualFree(ptr, size, MEM_DECOMMIT); }

internal void os_release(void *ptr, u64 size) {
  // NOTE(tijani): the size is not actually used in win32,
  // VirtualFree must be 0 to release the spcified page region
  // back to windows.
  // 'size' is therefore not used on windows but is necessary
  // for other OSes.
  VirtualFree(ptr, 0, MEM_RELEASE);
}

internal void *os_reserve_large(u64 size) {
  // NOTE(tijani): windows requires that large-page memory must
  // be reserved and commited as a single operation.
  void *result = VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT | MEM_LARGE_PAGES, PAGE_READWRITE);
  return result;
}

internal b32 os_commit_large(void *ptr, u64 size) {
  // NOTE(tijani): see os_reserve_large().
  return 1;
}

// Thread Info (implemented per-os)
internal u32 os_thread_id(void) {
  DWORD id = GetCurrentThreadId();
  return (u32)id;
}

// @TODO: make work when actually multithreading.
internal void os_set_thread_name(string8 name) {
  Temp scratch = scratch_begin(0, 0);

  scratch_end(scratch);
}

///////////////////////
// Aborting (implemented per-os)

internal void os_abort(s32 exit_code) { ExitProcess(exit_code); }

// TODO(tijani): Add argc and argv
internal void w32_entry_point_caller() {
  // TODO(tijani): Handle exception

  // NOTE(tijani): Do initialization stuff here before calling into the
  // "real" entry point

  // TODO(tijani): see if we can allow large pages

  SYSTEM_INFO sys_info;
  GetSystemInfo(&sys_info);
  {
    OS_SystemInfo *system_information = &os_w32_state.system_info;
    system_information->logical_processor_count = (u64)sys_info.dwNumberOfProcessors;
    system_information->page_size = sys_info.dwPageSize;
    system_information->large_page_size = GetLargePageMinimum();
    system_information->allocation_granularity = sys_info.dwAllocationGranularity;
  }

  // TODO(tijani): extract arguments

  // tijani: setup thread context
  local ThreadContext thread_context;
  thread_context_init_and_equip(&thread_context);

  // NOTE(tijani): call into the "real" entry point
  main_thread_base_entry_point();
}

#if BUILD_CONSOLE_INTERFACE
int wmain() {
  w32_entry_point_caller();
  return 0;
}
#elif BUILD_DEBUG
int main() {
  w32_entry_point_caller();
  return 0;
}
#else
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd) {
  w32_entry_point_caller();
  return 0;
}

#endif