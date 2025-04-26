/**
 * @file diagnostics.h
 * @author TF
 * @copyright Copyright (c) 2025 TF
 */
#pragma once

#include "compiler_internal.h"

typedef enum _DiagnosticPrintType
{
	PRINT_TYPE_NOTE,
	PRINT_TYPE_WARN,
	PRINT_TYPE_ERROR,
} DiagnosticPrintType;

void diagnostics_print_at_location(SourceSpan* span, const char* message, DiagnosticPrintType print_type);

void diagnostics_vnote_along_span(SourceSpan* location, const char* message, va_list args);

void diagnostics_vwarning_along_span(SourceSpan* location, const char* message, va_list args);

void diagnostics_verror_along_span(SourceSpan* location, const char* message, va_list args);
