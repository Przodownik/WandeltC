/**
 * @file platform_win32.h
 * @author TF
 * @copyright Copyright (c) 2023 TF
 */
#pragma once

typedef struct _Clock
{
	int64 begin_time;
	int64 frequency;
} Clock;

Clock clock_create(void);

void clock_restart_from_now(Clock* clock);

float64 clock_get_elapsed_time(Clock* clock);

bool run_linker(const char* args_string);
