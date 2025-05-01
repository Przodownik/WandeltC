#include "platform_win32.h"

typedef long long LONGLONG;

typedef struct _LARGE_INTEGER
{
	LONGLONG QuadPart;
} LARGE_INTEGER;

__declspec(dllimport) void __stdcall QueryPerformanceCounter(LARGE_INTEGER* lpPerformanceCount);
__declspec(dllimport) void __stdcall QueryPerformanceFrequency(LARGE_INTEGER* lpFrequency);

Clock clock_create(void)
{
	LARGE_INTEGER freq, start;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);
	return (Clock){start.QuadPart, freq.QuadPart};
}

Clock clock_restart_from_now(Clock* clock)
{
	LARGE_INTEGER freq, start;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);

	clock->begin_time = start.QuadPart;
	clock->frequency  = freq.QuadPart;
}

float64 clock_get_elapsed_time(Clock* clock)
{
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);
	return (now.QuadPart - clock->begin_time) / (double)clock->frequency;
}
