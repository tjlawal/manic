// Copyright Frost Gorilla, Inc. All Rights Reserved.

internal void main_thread_base_entry_point() {
  Temp scratch = scratch_begin(0, 0);
  ThreadNameF("[main thread]");

#if defined(OS_GFX_H) && !defined(OS_GFX_INIT_MANUAL)
  os_gfx_init();
#endif

  // TODO(tijani): parse command line

  // TODO(tijani): start profiler capture

  entry_point();

  // TODO(tijani): end profiler capture

  scratch_end(scratch);
}
internal void supplement_thread_base_entry_point(void (*entry_point)(void *params), void *params) {}

internal b32 update(void) {
#if OS_FEATURE_GRAPHICAL
  b32 result = frame();
#else
  b32 result = 0;
#endif
  return result;
}
