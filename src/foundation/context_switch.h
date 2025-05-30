#pragma once

/* 
Some opinions made at the foundation layer of this codebase that determines what operating systems, language standard,
and compiler versions, and CPU architecture are supported. These are meant to be minimum requirements this codebase 
promises to provide and guarantee within technical reason. Some will get dropped, 
(i.e., OS and compiler versions) with time as they become irrelevant or unneeded.

- Supported Architecture:
	- 64-bit processors only are supported.
	- x86_64, arm64 (or aarch64), and RISCV(when ever they materialize).
	- Big Endian and Little Endian support exist, but Little Endian is the default unless required.

- Language Standard: C99 and C++11.

- Supported operating systems are:
	- Windows 10 and above. If the need to support anything below that arises, then handle it then.
	- Linux. This is a never ending hole cause there's so many linux distros but the ones supported right now
		are the distros I am familiar with and in tandem, OLDEST LTS version as of this writing. If the minimum supported versions
		LTS dates has passed, assume they are longer supported.
	- Ubuntu 14.04 LTS (Legacy support ends Apr 2026) and above.
	- Debian 11 LTS (Legacy support ends August 2026) and above.
	- Fedora Linux
	- MacOS. Stubs are in place for its support but pending I acquire a mac or need arise, whichever comes first, stub it shall remain.

- Supported compilers are:
	- MSVC C/C++ compiler - 2017 and above. (NOTE: As of the time of writing, I dont have access to / can't find a 
	community version of the compiler less than 19.43, so ensuring support with other versions is gonna be fishy except I buy a subscription)
	- GCC 7.5  is minimum supported compiler.
	- Clang 3.3 is the minimum supported compiler.
*/

#if defined(__clang__)
	#define COMPILER_CLANG 1
#elif defined(_MSC_VER)
	#define COMPILER_MSVC 1
#elif defined(__GNUC__)
	#define COMPILER_GCC 1
#else
	#error "The compiler is not supported."
#endif

#if defined(_WIN32) || defined(_WIN64)
	#define OS_WINDOWS 1
#elif defined(__linux__)
	#define OS_LINUX 1
#elif defined(__APPLE__) && defined(__MACH__)
	#define OS_MACOS 1
#else
	#error "This operating system is not supported."
#endif

#if defined(_M_IX86) || defined(__i386__)
	#define CPU_X86 1
	#warning "x86 (32-bit) CPU used, this is not supported."
#elif (__amd64) || defined(__x86_64) || defined(_M_AMD64) || defined(__aarch64__) || defined(_M_ARM64)
	#define CPU_X64 1
#else
	#error "CPU architecture is not supported."
#endif

// read-only segment
#if COMPILER_MSVC || (COMPILER_CLANG && OS_WINDOWS)
  #pragma section(".rdata$", read)
  #define read_only __declspec(allocate(".rdata$"))
#elif (COMPILER_CLANG && OS_LINUX)
  #define read_only __attribute__((section(".rodata")))
#endif

// Build Options
#if !defined(BUILD_DEBUG)
  #define BUILD_DEBUG 1
#endif

#if !defined(BUILD_RELEASE)
	#define BUILD_RELEASE 0
#endif

#if !defined(BUILD_ENTRY_DEFINING_UNIT)
  #define BUILD_ENTRY_DEFINING_UNIT 1
#endif

#if !defined(BUILD_VERSION_MAJOR)
  #define BUILD_VERSION_MAJOR 0
#endif

#if !defined(BUILD_VERSION_MINOR)
  #define BUILD_VERSION_MINOR 0
#endif

#if !defined(BUILD_VERSION_PATCH)
  #define BUILD_VERSION_PATCH 0
#endif

#if defined(OS_WINDOWS)
	#define _CRT_SECURE_NO_WARNINGS 1
#endif

#define BUILD_VERSION_STRING_LITERAL Stringify(BUILD_VERSION_MAJOR) "." Stringify(BUILD_VERSION_MINOR) "." Stringify(BUILD_VERSION_PATCH)

#if BUILD_DEBUG
  #define BUILD_MODE_STRING_LITERAL_APPEND " [Debug]"
#elif BUILD_RELEASE
	#define BUILD_MODE_STRING_LITERAL_APPEND " [Release]"
#else 
  #define BUILD_MODE_STRING_LITERAL_APPEND ""
#endif

#if defined(BUILD_SVN_REVISION)
  #define BUILD_SVN_REVISION_STRING_LITERAL_APPEND " [Revision: " Stringify(BUILD_SVN_REVISION) "]"
#else
  #define BUILD_SVN_REVISION_STRING_LITERAL_APPEND ""
#endif

#if !defined(BUILD_TITLE)
  #define BUILD_TITLE "Untitled"
#endif

#if !defined(BUILD_RELEASE_PHASE_STRING_LITERAL)
  #define BUILD_RELEASE_PHASE_STRING_LITERAL "ALPHA"
#endif

#if !defined(BUILD_ISSUES_LINK_STRING_LITERAL)
  #define BUILD_ISSUES_LINK_STRING_LITERAL "issues@tlawal.org"
#endif

#define BUILD_TITLE_STRING_LITERAL BUILD_TITLE " (" BUILD_VERSION_STRING_LITERAL " " BUILD_RELEASE_PHASE_STRING_LITERAL ") - " __DATE__ "" BUILD_SVN_REVISION_STRING_LITERAL_APPEND BUILD_MODE_STRING_LITERAL_APPEND

// Zero out all options

// Compiler
#if !defined(COMPILER_MSVC)
  #define COMPILER_MSVC 0
#endif
#if !defined(COMPILER_GCC)
  #define COMPILER_GCC 0
#endif
#if !defined(COMPILER_CLANG)
  #define COMPILER_CLANG 0
#endif

// OS	
#if !defined(OS_WINDOWS)
  #define OS_WINDOWS 0
#endif

#if !defined(OS_LINUX)
	#define OS_LINUX 0
#endif

#if !defined(OS_MACOS)
	#define OS_MACOS 0
#endif
	
#if !defined(CPU_X86)
	#define CPU_X86 0
#endif

#if !defined(CPU_X64)
	#define CPU_X64 0
#endif