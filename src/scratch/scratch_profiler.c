// Copyright Frost Gorilla, Inc. All Rights Reserved.

#define BUILD_CONSOLE_INTERFACE 1

// clang-format off
#include "base/base_inc.h"
#include "os/os_inc.h"


#include "base/base_inc.c"
#include "os/os_inc.c"
// clang-format on

#if COMPILER_MSVC
  #include <intrin.h> // for __rdtsc()

#elif COMPILER_CLANG
  #include <x86intrin.h>
#else
  #error x86 intrinsic not available on your compiler.
#endif

static u64 os_timer_freq() {
  LARGE_INTEGER freq;
  QueryPerformanceFrequency(&freq);
  return freq.QuadPart;
}

static u64 os_time_counter(void) {
  LARGE_INTEGER counter;
  QueryPerformanceCounter(&counter);
  return counter.QuadPart;
}

static u64 read_cpu_timer(void) { return __rdtsc(); }

void entry_point(void) {
  u64 os_freq = os_timer_freq();
  printf("OS Frequency: %llu\n", os_freq);

  u64 os_start = os_time_counter();
  u64 os_end = 0;
  u64 os_elapsed = 0;
  u64 cpu_timer_start = read_cpu_timer();

  while (os_elapsed < os_freq) {
    os_end = os_time_counter();
    os_elapsed = os_end - os_start;
  }

  u64 cpu_timer_end = read_cpu_timer();
  u64 cpu_elapsed = cpu_timer_end - cpu_timer_start;

  printf("OS Timer: %llu -> %llu = %llu elapsed\n", os_start, os_end, os_elapsed);
  printf("OS Seconds: %.4f\n", (f64)os_elapsed / (f64)os_freq);
  printf("CPU Timer: %llu -> %llu = %llu elapsed\n", cpu_timer_start, cpu_timer_end, cpu_elapsed);
}