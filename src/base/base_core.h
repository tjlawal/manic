// Copyright Frost Gorilla, Inc. All Rights Reserved.

#ifndef BASE_TYPES_H
#define BASE_TYPES_H

// NOTE(tijani:) foreign includes, try to trim down as much as possible.
//  THe goal is to be unreliant on the c-runtime libs
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Keywords Macros
#define global   static
#define internal static
#define local    static

// Base Types
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef s8 b8;
typedef s16 b16;
typedef s32 b32;
typedef s64 b64;

typedef float f32;
typedef double f64;

// Constants
global u16 max_u16 = 0xffff;
global u32 max_u32 = 0xffffffff;

// Bitmasks
global const u32 bitmask1 = 0x00000001;
global const u32 bitmask2 = 0x00000003;
global const u32 bitmask3 = 0x00000007;
global const u32 bitmask4 = 0x0000000f;
global const u32 bitmask5 = 0x0000001f;
global const u32 bitmask6 = 0x0000003f;
global const u32 bitmask7 = 0x0000007f;
global const u32 bitmask8 = 0x000000ff;
global const u32 bitmask9 = 0x000001ff;
global const u32 bitmask10 = 0x000003ff;

global const u32 bit8 = (1 << 7);
global const u32 bit25 = (1 << 24);
global const u32 bit31 = (1 << 30);
global const u32 bit32 = (1 << 31);

// Units
#define KB(n)       (((u64)(n)) << 10)
#define MB(n)       (((u64)(n)) << 20)
#define GB(n)       (((u64)(n)) << 30)
#define TB(n)       (((u64)(n)) << 40)
#define Thousand(n) ((n) * 1000)
#define Million(n)  ((n) * 1000000)
#define Billion(n)  ((n) * 1000000000)

// Thread local storage shenanigans
#if COMPILER_MSVC
  #define thread_static __declspec(thread)
#elif COMPILER_CLANG || COMPILER_GCC
  #define thread_static __thread
#endif

// Branch prediction hints
#if defined(__clang__)
  #define expect(expr, val) __builtin_expect((expr), (val))
#else
  #define expect(expr, val) (expr)
#endif

#define likely(expr)   expect(expr, 1)
#define unlikely(expr) expect(expr, 0)

// C++ linking shenanigans
#if LANG_CPP
  #define C_LINK_BEGIN extern "C" {
  #define C_LINK_END   }
  #define C_LINK       extern "C"
#else
  #define C_LINK_BEGIN
  #define C_LINK_END
  #define C_LINK
#endif

// Assertions

#if COMPILER_MSVC
  #define trap() __debugbreak()
#elif COMPILER_CLANG || COMPILER_GCC
  #define trap() __builtin_trap()
#else
  #error Unknown trap intrinsic for this compiler.
#endif

#define assert_always(x)                                                                                               \
  do {                                                                                                                 \
    if (!(x)) {                                                                                                        \
      trap();                                                                                                          \
    }                                                                                                                  \
  } while (0)

// tijani: Address Sanitizer

#if COMPILER_MSVC
  #if defined(__SANITIZE_ADDRESS__)
    #define ASAN_ENABLED 1
    #define NO_ASAN      __declspec(no_sanitize_address)
  #else
    #define NO_ASAN
  #endif

#elif COMPILER_CLANG
  #if defined(__has_feature)
    #if __has_feature(address_sanitizer) || defined(__SANITIZE_ADDRESS__)
      #define ASAN_ENABLED 1
    #endif
    #define NO_ASAN __attribute__((no_sanitize("address")))
  #endif
#else
  #define NO_ASAN
#endif

#if ASAN_ENABLED
  #pragma comment(lib, "clang_rt.asan-x86_64.lib")
C_LINK void __asan_poison_memory_region(void const volatile *addr, size_t size);
C_LINK void __asan_unpoison_memory_region(void const volatile *addr, size_t size);
  #define AsanPoisonMemoryRegion(addr, size)   __asan_poison_memory_region((addr), (size))
  #define AsanUnpoisonMemoryRegion(addr, size) __asan_unpoison_memory_region((addr), (size))
#else
  #define AsanPoisonMemoryRegion(addr, size)   ((void)(addr), (void)(size))
  #define AsanUnpoisonMemoryRegion(addr, size) ((void)(addr), (void)(size))
#endif

// tijani: Memory Cleaning Ops

#define memory_copy(dest, src, src_size) memmove((dest), (src), (src_size))
#define memory_copy_array(dest, size)    memory_copy((dest), (size), sizeof(dest))
#define memory_copy_struct(dest, size)   memory_copy((dest), (size), sizeof(*(dest)))

#define memory_zero(dest, count)           memset((dest), 0, (count))
#define memory_zero_array(array)           memory_zero(array, sizeof(array))
#define memory_zero_struct(struct_to_zero) memory_zero((struct_to_zero), sizeof(*(struct_to_zero)))

// tijani: Max, Min, Clamps

#define MAX(A, B)       (((A) > (B)) ? (A) : (B))
#define MIN(A, B)       (((A) < (B)) ? (A) : (B))
#define CLAMP_TOP(A, X) MIN(A, X)
#define CLAMP_BOT(X, B) MAX(X, B)
#define CLAMP(A, X, B)  (((A) < (X)) ? (X) : ((A) > (B)) ? (B) : (A))

// tijani: Swap types
#define swap(a, b, type)                                                                                               \
  do {                                                                                                                 \
    type temp = *(a);                                                                                                  \
    *(a) = *(b);                                                                                                       \
    *(b) = temp;                                                                                                       \
  } while (0)

// Utilities
// inline internal void swap_ints(s32 *a, s32 *b) {
//  s32 temp = *a;
//  *a = *b;
//  *b = temp;
//}

// inline internal void swap_floats(f32 *a, f32 *b) {
//   f32 temp = *a;
//   *a = *b;
//   *b = temp;
// }

// tijani: Type Alignment
#if COMPILER_MSVC
  #define AlignOf(type) __alignof(type)
#elif COMPILER_CLANG
  #define AlignOf(type) __alignof(type)
#elif COMPILER_GCC
  #define AlignOf(type) __alignof__(type)
#else
  #error alignof not defined for this compiler.
#endif

// Alignment Macros
#define align_pow2(x, b) (((x) + (b) - 1) & (~((b) - 1)))

// Generic Linked List and Queues macros
// NOTE(tijani): The benefits this present outweight their drawbacks.
// Take extra care when debugging things that interact with them.

#define CheckNil(nil, ptr) ((ptr) == 0 || (ptr) == (nil))
#define SetNil(nil, ptr)   ((ptr) = (nil))

// NOTE(tijani): Singly linked list operations. These are singly headed, stacks.
#define SLL_StackPushNext(first, node, next) ((node)->next = (first), (first) = (node))
#define SLL_StackPopNext(first, next)        ((first) = (first)->next)

// NOTE(tijani): helpers
#define SLL_StackPush(first, node) SLL_StackPushNext(first, node, next)
#define SLL_StackPop(first)        SLL_StackPopNext(first, next)

// NOTE(tijani): Doubly linked list operations

// NOTE(tijani): Breakdown:-
// Either, check if it is an empty list, if not insert at the front of the list, else if not insert at the
// end of the list, else insert in the middle.
#define DLL_InsertNextPrevZero(nil, first, last, position, node, next, previous)                                       \
  (CheckNil(nil, first) ? ((first) = (last) = (node), SetNil(nil, (node)->next), SetNil(nil, (node)->previous))        \
   : CheckNil(nil, position)                                                                                           \
       ? ((node)->next = (first), (first)->previous = (node), (first) = (node), SetNil(nil, (node)->previous))         \
   : ((position) == (last))                                                                                            \
       ? ((last)->next = (node), (node)->previous = (last), (last) = (node), SetNil(nil, (node)->next))                \
       : ((!CheckNil(nil, position) && CheckNil(nil, (position)->next)) ? (0) : ((position)->next->previous = (node)), \
          (node)->next = (position)->next, (position)->next = (node), (node)->previous = (position)))

#define DLL_PushBackNextPrevZero(nil, first, last, node, next, previous)                                               \
  DLL_InsertNextPrevZero(nil, first, last, last, node, next, previous)

#define DLL_PushFrontNextPrevZero(nil, first, last, node, next, previous)                                              \
  DLL_InsertNextPrevZero(nil, last, first, first, node, previous, next)

// NOTE(tijani): Breakdown:-
// Upate first if needed, update last if needed, update previous link, update next link
#define DLL_RemoveNextPrevZero(nil, first, last, node, next, previous)                                                 \
  (((node) == (first) ? (first) = (node)->next : (0)), ((node) == (last) ? (last) = (last)->previous : (0)),           \
   (CheckNil(nil, (node)->previous) ? (0) : ((node)->previous->next = (node)->next)),                                  \
   (CheckNil(nil, (node)->next) ? (0) : ((node)->next->previous = (node)->previous)))

// NOTE(tijani:) Doubly-linked list operations
#define DLL_Insert(first, last, position, node) DLL_InsertNextPrevZero(0, first, last, position, node, next, previous)
#define DLL_PushBack(first, last, node)         DLL_PushBackNextPrevZero(0, first, last, node, next, previous)
#define DLL_PushFront(first, last, node)        DLL_PushFrontNextPrevZero(0, first, last, node, next, previous)
#define DLL_Remove(first, last, node)           DLL_RemoveNextPrevZero(0, first, last, node, next, previous)

//////////////////////////////////////
// tijani: Miscellaneous
#define array_count(array_to_count) (sizeof(array_to_count) / sizeof((array_to_count)[0]))
#define stringify_(string)          #string
#define stringify(string)           stringify_(string)

// Note(tijani): dynamic array library from gustavopezzi [dynamic array]:
// https://github.com/gustavopezzi/dynamicarray
// @remove: make better implementation.
#define array_push(array, value)                                                                                       \
  do {                                                                                                                 \
    (array) = array_hold((array), 1, sizeof(*(array)));                                                                \
    (array)[array_length(array) - 1] = (value);                                                                        \
  } while (0);

void *array_hold(void *array, int count, int item_size);
int array_length(void *array);
void array_free(void *array);

#endif // BASE_TYPES_H
