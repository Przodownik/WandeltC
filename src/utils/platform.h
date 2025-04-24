/**
 * @file platform.h
 * @author TF
 * @copyright Copyright (c) 2023 TF
 */
#pragma once

#ifdef WDT_PLATFORM_WINDOWS

	#include "platform/platform_win32.h"

#else

	#error "Platform not yet supported!";

#endif
