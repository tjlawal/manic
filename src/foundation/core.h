#pragma once

// Linkage stuff
#define C_LINKAGE extern "C"

#if defined(BUILD_RELEASE)
	#if defined(COMPILER_MSVC)
		#define FORCE_INLINE __forceinline
	#elif defined(COMPILER_CLANG) || defined(COMPILER_GCC)
		#define FORCE_INLINE __attribute__((always_inline))
	#endif
#else
	#if defined(COMPILER_MSVC)
		#define FORCE_INLINE inline
	#elif defined(COMPILER_CLANG) || defined(COMPILER_GCC)
		#define FORCE_INLINE inline
	#endif
#endif

// Debugger trap
#if COMPILER_MSVC
	#define DEBUGBREAK __debugbreak()
#elif COMPILER_CLANG || COMPILER_GCC
	#define DEBUGBREAK __builtin_trap()
#else
	#error "Unknown debug break intrinsic for this compiler."
#endif

// DateTime, DenseTime stuff
typedef u64 DenseTime;

enum WeekDay : u8 {
	WeekDay_Sun,
	WeekDay_Mon,
	WeekDay_Tue,
	WeekDay_Wed,
	WeekDay_Thu,
	WeekDay_Fri,
	WeekDay_Sat,

	WeekDay_COUNT,
};

enum Month : u8{
  Month_Jan,
  Month_Feb,
  Month_Mar,
  Month_Apr,
  Month_May,
  Month_Jun,
  Month_Jul,
  Month_Aug,
  Month_Sep,
  Month_Oct,
  Month_Nov,
  Month_Dec,

  Month_COUNT,
};

struct DateTime {
	u16 micro_second; // 10^-6
	u16 milli_second; // 10^-3
	u16 second; 			// [0-60]
	u16 minute; 			// [0-59]
	u16 hour;					// [0-24]
	u16 day;					// [0-30]

	union {
		WeekDay week_day;
		u32 wday;
	};

	union {
		Month month;
		u32 mon;
	};

	u32 year; // 1 = 1 CE, 0 = 1 BCE
};


// @TODO: Pass message to assertion
#define AssertAlways(x)	do { if (!(x)) { DEBUGBREAK; } } while (0)
#if BUILD_DEBUG
	#define Assert(x) AssertAlways(x)
#else
	#define Assert(x) (void)(x)
#endif

// Branch prediction hints
#if defined(__clang__)
	#define EXPECT(expr, val) __builtin_expect((expr), (val))
#else
	#define EXPECT(expr, val) (expr)
#endif
#define LIKELY(expr)   EXPECT(expr, 1)
#define UNLIKELY(expr) EXPECT(expr, 0)

// Memory Cleaning Ops
#define MemoryCopy(dest, src, src_size)  memmove((dest), (src), (src_size))
#define MemoryCopyArray(dest, size)      MemoryCopy((dest), (size), sizeof(dest))
#define MemoryCopyStruct(dest, size)     MemoryCopy((dest), (size), sizeof(*(dest)))

#define MemoryZero(dest, count)          memset((dest), 0, (count))
#define MemoryZeroArray(array)           MemoryZero(array, sizeof(array))
#define MemoryZeroStruct(struct_to_zero) MemoryZero((struct_to_zero), sizeof(*(struct_to_zero)))

// Alignment Macros
#define AlignPow2(x, b)   (((x) + (b) - 1) & (~((b) - 1)))  
#define Compose64Bit(a, b) ((((u64)a) << 32) | ((u64)b));

// LinkedList Macros
#define _SLLPush(first, node, next) ((node)->next = (first), (first) = (node))
#define _SLLPop(first, next)	((first) = (first)->next)

// Helper macros
#define SLLPush(first, node) _SLLPush(first, node, next)
#define SLLPop(first) _SLLPop(first, next)

// max, min, etc
#define max(A, B)       (((A) > (B)) ? (A) : (B))
#define min(A, B)       (((A) < (B)) ? (A) : (B))
#define clamp_min(A, X) min(A, X)
#define clamp_max(X, B) max(X, B)

// Address Sanitization
#if COMPILER_MSVC
  #if defined(__SANITIZE_ADDRESS__)
    #define ASAN_ENABLED 1
    #define NO_ASAN	__declspec(no_sanitize_address)
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
	#if BUILD_RELEASE
		#error "This is not supported nor does it make sense, do a debug build."
	#endif
	#if COMPILER_MSVC
		// At the time of writing, Visual Studio Community Edition 2022 V17.14.4 prefers ASAN to be dynamically linked in but everything is statically linked including the C runtime because it is easy to deploy without hunting for VC++ redistributables. Until I can figure out how to make MSVC to play nice with the asan dll in run_tree it doesn't work. - Tijani 06/10/2025
		#error "Please use clang." // Crash and burn!
	#endif
	#if COMPILER_CLANG
		#if __clang_major__ <= 18
			#pragma comment(lib, "clang_rt.asan-x86_64.lib")
		#endif
	#endif
	C_LINKAGE void __asan_poison_memory_region(void const volatile *addr, size_t size);
	C_LINKAGE void __asan_unpoison_memory_region(void const volatile *addr, size_t size);
  #define AsanPoisonMemoryRegion(addr, size)   __asan_poison_memory_region((addr), (size))
  #define AsanUnpoisonMemoryRegion(addr, size) __asan_unpoison_memory_region((addr), (size))
#else
  #define AsanPoisonMemoryRegion(addr, size)   ((void)(addr), (void)(size))
  #define AsanUnpoisonMemoryRegion(addr, size) ((void)(addr), (void)(size))
#endif

///////////////////////////////////////////// Temporary, will delete soonest ///////////////////////////////////////
#define CheckNil(nil, ptr) ((ptr) == 0 || (ptr) == (nil))
#define SetNil(nil, ptr)   ((ptr) = (nil))

// NOTE: Doubly linked list operations
// NOTE: Breakdown:-
// Either, check if it is an empty list, if not insert at the front of the list,
// else if not insert at the end of the list, else insert in the middle.
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

	// NOTE: Breakdown:-
	// Upate first if needed, update last if needed, update previous link, update
	// next link
	#define DLL_RemoveNextPrevZero(nil, first, last, node, next, previous)                                                 \
  (((node) == (first) ? (first) = (node)->next : (0)), ((node) == (last) ? (last) = (last)->previous : (0)),           \
		(CheckNil(nil, (node)->previous) ? (0) : ((node)->previous->next = (node)->next)),                                  \
		(CheckNil(nil, (node)->next) ? (0) : ((node)->next->previous = (node)->previous)))

	// NOTE Doubly-linked list operations
	#define DLL_Insert(first, last, position, node) DLL_InsertNextPrevZero(0, first, last, position, node, next, previous)
	#define DLL_PushBack(first, last, node)         DLL_PushBackNextPrevZero(0, first, last, node, next, previous)
	#define DLL_PushFront(first, last, node)        DLL_PushFrontNextPrevZero(0, first, last, node, next, previous)
	#define DLL_Remove(first, last, node)           DLL_RemoveNextPrevZero(0, first, last, node, next, previous)

#define ARRAY_RAW_DATA(arrray) ((int *)(array) - 2)
#define ARRAY_OCCUPIED(array)	 (ARRAY_RAW_DATA(array)[1])

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Miscl
#define ArrayCount(arr) (sizeof(arr) / sizeof((arr)[0]))

namespace Starlight {
	namespace Foundation {
		FORCE_INLINE size_t is_pow2(size_t val) { return (val & (val - 1)) == 0; }

		FORCE_INLINE size_t align_pow2(size_t value, size_t alignment) {
			Assert(is_pow2(alignment));	
			size_t mask = alignment - 1;
			return (value + mask) & ~mask;
		}

		template<typename T> 
		FORCE_INLINE T clamp(T value, T min, T max) {
			if(value < min)  
				return min; 
			else if(value > max) 
				return max; 
			else 
				return value;
		};

		template<typename T>
		FORCE_INLINE void swap(T& a, T& b) {
			T __t = a;
			a = b;
			b = __t;
		}

		template<typename T>
		FORCE_INLINE void swap_ptrs(T* a, T* b) {
			T temp = *(a);
			*(a) = *(b);
			*(b) = temp;
		}

		FORCE_INLINE s32 array_length(void *array) {
			return (array != nullptr) ? ARRAY_OCCUPIED(array) : 0;
		}

		internal u16 cast_u16_from_u32(u32 x);
		internal u32 saturate_u32_from_u64(u64 x); // Fill a u32 to the brim with infromation from a u64.

		// Time functions
		internal DateTime date_time_from_dense_time(DenseTime time);
		internal DenseTime dense_time_from_date_time(DateTime time);

	}
} 