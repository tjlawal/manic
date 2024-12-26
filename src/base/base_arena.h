// Copyright Frost Gorilla, Inc. All Rights Reserved.

#ifndef BASE_ARENA_H
#define BASE_ARENA_H

// clang-format off
/* NOTE(tijani): READ

	Heavily inspired by Epic Games Tools arena allocatior.
  Arena allocator simulates how stack memory works.
  This allows us the ability to allocate a reasonable amount of memory
  at startup and only use that allocated amount of memory throughout
  the lifetime of the application.

  Obviously this still uses "malloc, free, VirtualAlloc, VirtualFree"
  but instead of having to request and freeing memory during runtime,
  we create an environment where we have as much memory we could ever
  need and them portion it out in small chunk as needed.

  This has multiple benefits that would make itself glaring throughout
  the codebase but the gist of it is that we have one large block of
  memory throughout the lifetime, then application specific code that
  could need memory grab a chunck memory from the big block of memory
  we have reserved with the operating system at startup do whatever
  operation it needs to do, then return that block of memory back to
  the arena.

												Arena Memory Layout
                                    
Arena Root                        Active Block
┌──────────────────────┐          ┌──────────────────────┐
│ previous -> null     │          │     HEADER           │
│ current  						 |--------> ├──────────────────────┤
│ flags                │          │                      │
│ commit_size          │          │    User Memory       │
│ reserve_size         │          │                      │
│ base_position        │          │    position →  ●     │
│ position             │          │                      │
│ commit               │          │    commit →   ●      │
│ reserve              │          │                      │
│ free_size            │          │    reserve →  ●      │
│ free_last ───────┐   │          │                      │
└──────────────────┼───┘          └──────────────────────┘
									 │
									 │              Free List
									 │     ┌──────────────────────┐
									 │     │   Freed Block 1      │
									 └────►├──────────────────────┤
												 │                      │
												 │     (Available)      │
									       │                      │
									       └──────────────┬───────┘
																				│
													┌─────────────∇───────┐  
													│   Freed Block 2     │
													├─────────────────────┤
													│                     │
													│     (Available)     │
													│                     │
													└──────────────┬──────┘
																				 │
																				NULL
*/
// clang-format on

// Constants
#define ARENA_HEADER_SIZE 128

// Flags
// ArenaFlag_NoChain - Arenas are chained by default, this flag tells it
// otherwise. ArenaFlag_LargePages - Use large pages.
typedef u64 ArenaFlags;
enum {
  ArenaFlag_NoChain = (1 << 0),
  ArenaFlag_LargePages = (1 << 1),
};

// NOTE(tijani): optional_preallocated_buffer is there in the event you need to
// use a specific block of memory.
typedef struct ArenaParams ArenaParams;
struct ArenaParams {
  ArenaFlags flags;
  u64 reserve_size;
  u64 commit_size;
  void *optional_preallocated_buffer;
};

typedef struct Arena Arena;
struct Arena {
  Arena *previous; // Previous arena in the chain
  Arena *current;  // Current arena in the chain
  ArenaFlags flags;
  u64 commit_size;
  u64 reserve_size;
  u64 base_position;
  u64 position;
  u64 commit;
  u64 reserve;

  // NOTE(tijani): this represents recently freed blocks of memory in the arena.
  // this is pointed to by the previous pointer. Newly freed blocks of memory
  // are added in a last-in-first-out fashion, essentially a stack structure.
  // Diagram to help understand:
  //	┌─────────────┐
  //	│ free_last   │──┐
  //	└─────────────┘  │
  //									 ▼
  //							[Freed Block 3]
  //									 │
  //                   ▼
  //							[Freed Block 2]
  // 							     │
  // 									 ▼
  //						  [Freed Block 1]
  // 									 │
  // 									 ▼
  // 									null
#if ARENA_FREE_LIST
  u64 free_size;
  Arena *free_last;
#endif
};

typedef struct Temp Temp;
struct Temp {
  Arena *arena;
  u64 position;
};

// NOTE(tijani): Global default
global u64 arena_default_reserve_size = MB(64);
global u64 arena_default_commit_size = KB(64);
global ArenaFlags arena_default_flags = 0;

// Create, Destroy
internal Arena *arena_alloc_(ArenaParams *params);
// NOTE(tijani): default values if nothing is passed
#define arena_alloc(...)                                                                                               \
  arena_alloc_(&(ArenaParams){.reserve_size = arena_default_reserve_size,                                              \
                              .commit_size = arena_default_commit_size,                                                \
                              .flags = arena_default_flags,                                                            \
                              __VA_ARGS__})
internal void arena_release(Arena *arena);

// NOTE(tijani): Arena core push, pop, position operations
internal void *arena_push(Arena *arena, u64 size_to_push, u64 alignment);
internal u64 arena_position(Arena *arena);
internal void arena_pop_to(Arena *arena, u64 position);

// NOTE(tijani): Arena core operation helpers
internal void arena_clear(Arena *arena);
internal void arena_pop_off(Arena *arena, u64 amount);

// NOTE(tijani): temporary arena scopes
internal Temp temp_begin(Arena *arena);
internal void temp_end(Temp temp);

// NOTE(tijani): helpers
#define push_array_no_zero_aligned(a, T, c, align) (T *)arena_push((a), sizeof(T) * (c), (align))
#define push_array_aligned(a, T, c, align)         (T *)memory_zero(push_array_no_zero_aligned(a, T, c, align), sizeof(T) * (c))
#define push_array_no_zero(arena, Type, count)     push_array_no_zero_aligned(arena, Type, count, max(8, AlignOf(Type)))
#define push_array(a, T, c)                        push_array_aligned(a, T, c, max(8, AlignOf(T)))

#endif // BASE_ARENA_H
