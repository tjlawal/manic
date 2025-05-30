#pragma once

// @revise: Foreign includes, replace with homegrown versions
#include <stdint.h> // For integer types
#include <math.h>   // sin, cos, tan, round
#include <stdio.h>  // printf

//#include <stdarg.h>
//#include <stdlib.h>
//#include <string.h>

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
global u64 max_u64 = 0xffffffffffffffff;

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

#define Stringify_(string)          #string
#define Stringify(string)           Stringify_(string)