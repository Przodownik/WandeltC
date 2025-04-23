/**
 * @file defines.h
 * @author TF
 * @copyright Copyright (c) 2023 TF
 */
#pragma once

// Unsigned int types.
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

// Signed int types.
typedef signed char int8;
typedef signed short int16;
typedef signed int int32;
typedef signed long long int64;

// Floating point types
typedef float float32;
typedef double float64;

// Boolean types
typedef int bool32;

static_assert(sizeof(uint8) == 1, "uint8 is not 1 byte");
static_assert(sizeof(uint16) == 2, "uint16 is not 2 bytes");
static_assert(sizeof(uint32) == 4, "uint32 is not 4 bytes");
static_assert(sizeof(uint64) == 8, "uint64 is not 8 bytes");

static_assert(sizeof(int8) == 1, "int8 is not 1 byte");
static_assert(sizeof(int16) == 2, "int16 is not 2 bytes");
static_assert(sizeof(int32) == 4, "int32 is not 4 bytes");
static_assert(sizeof(int64) == 8, "int64 is not 8 bytes");

static_assert(sizeof(float32) == 4, "float32 is not 4 bytes");
static_assert(sizeof(float64) == 8, "float64 is not 8 bytes");

static_assert(sizeof(bool32) == 4, "bool32 is not 4 bytes");

#if defined(WDT_PLATFORM_WINDOWS)
	#define DEBUG_BREAK() __debugbreak()
#elif defined(WDT_PLATFORM_LINUX) || defined(WDT_PLATFORM_MACOS)
	#include <signal.h>
	#define DEBUG_BREAK() raise(SIGTRAP)
#else
	#define DEBUG_BREAK() ((void)0)
	#error "Debug break not supported on this platform"
#endif

#define VA_OPTIONAL_EXPANSION(...) __VA_OPT__(, )##__VA_ARGS__

#define ANSI_COLOR_BLACK      "\x1b[30m"
#define ANSI_COLOR_RED        "\x1b[31m"
#define ANSI_COLOR_GREEN      "\x1b[32m"
#define ANSI_COLOR_YELLOW     "\x1b[33m"
#define ANSI_COLOR_BLUE       "\x1b[34m"
#define ANSI_COLOR_MAGENTA    "\x1b[35m"
#define ANSI_COLOR_CYAN       "\x1b[36m"
#define ANSI_COLOR_WHITE      "\x1b[37m"
#define ANSI_BG_COLOR_BLACK   "\x1b[40m"
#define ANSI_BG_COLOR_RED     "\x1b[41m"
#define ANSI_BG_COLOR_GREEN   "\x1b[42m"
#define ANSI_BG_COLOR_YELLOW  "\x1b[43m"
#define ANSI_BG_COLOR_BLUE    "\x1b[44m"
#define ANSI_BG_COLOR_MAGENTA "\x1b[45m"
#define ANSI_BG_COLOR_CYAN    "\x1b[46m"
#define ANSI_BG_COLOR_WHITE   "\x1b[47m"
#define ANSI_COLOR_RESET      "\x1b[0m"

#define nullptr NULL

#define KB(x) ((x) * 1024)
#define MB(x) (KB(x) * 1024)
#define GB(x) (MB(x) * 1024)

#define TRACE(fmt, ...) fprintf(stdout, fmt __VA_OPT__(, )##__VA_ARGS__)
#define ERROR(fmt, ...) fprintf(stderr, ANSI_COLOR_RED fmt ANSI_COLOR_RESET __VA_OPT__(, )##__VA_ARGS__)

#define ASSERT(condition, fmt, ...)                 \
	do                                              \
	{                                               \
		if (!(condition))                           \
		{                                           \
			ERROR(fmt __VA_OPT__(, )##__VA_ARGS__); \
			DEBUG_BREAK();                          \
			exit(-1);                               \
		}                                           \
	} while (0)

#define VERIFY(condition, fmt, ...)                 \
	do                                              \
	{                                               \
		if (!(condition))                           \
		{                                           \
			ERROR(fmt __VA_OPT__(, )##__VA_ARGS__); \
			DEBUG_BREAK();                          \
			exit(-1);                               \
		}                                           \
	} while (0)
