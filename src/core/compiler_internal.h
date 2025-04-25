/**
 * @file compiler_internal.h
 * @author TF
 * @copyright Copyright (c) 2025 TF
 */
#pragma once

#include "core/token.h"

ArenaAllocator string_allocator;

HashMap symbol_table;

typedef struct _SourceSpan
{
	const File* source_file;
	uint32 row;
	uint32 column;
	uint32 length;
} SourceSpan;

typedef struct _Token
{
	TokenType type;
	const char* lexeme;
	SourceSpan source_span;
} Token;

void compiler_internal_initialize(void);

void compiler_internal_shutdown(void);
