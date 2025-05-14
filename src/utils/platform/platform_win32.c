#include "platform_win32.h"

#include <windows.h>

Clock clock_create(void)
{
	LARGE_INTEGER freq, start;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);
	return (Clock){start.QuadPart, freq.QuadPart};
}

void clock_restart_from_now(Clock* clock)
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

bool run_linker(const char* args_string)
{
	STARTUPINFOA si        = {0};
	PROCESS_INFORMATION pi = {0};
	si.cb                  = sizeof(si);

	if (!CreateProcessA(NULL,        // Application name (NULL means use command line)
	                    args_string, // Command line
	                    NULL,        // Process attributes
	                    NULL,        // Thread attributes
	                    FALSE,       // Inherit handles
	                    0,           // Creation flags
	                    NULL,        // Environment (NULL = inherit)
	                    NULL,        // Current directory (NULL = inherit)
	                    &si,         // Startup info
	                    &pi))        // Process info
	{
		VERROR("ERROR: CreateProcess failed (%lu)\n", GetLastError());
		return false;
	}

	// Wait for the process to finish
	WaitForSingleObject(pi.hProcess, INFINITE);

	// Check exit code
	DWORD exitCode;
	GetExitCodeProcess(pi.hProcess, &exitCode);
	if (exitCode != 0)
	{
		VERROR("Linker exited with code %lu\n", exitCode);
		return false;
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return true;
}
