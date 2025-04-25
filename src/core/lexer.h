/**
 * @file lexer.h
 * @author TF
 * @copyright Copyright (c) 2025 TF
 */
#pragma once

#include "core/compiler_internal.h"

typedef struct _Lexer
{
	const char* file_begin_char;      // The start of the file being lexed
	const char* lexing_start_char;    // The start of the current token being lexed
	const char* current_char;         // The current character being lexed
	const char* line_start_char;      // The start of the current line being lexed
	const char* start_row_start_char; // The start of the current row being lexed
	uint32 current_row_number;        // The current row number in the file being lexed
	uint32 lexing_start_row_number;   // The row number where the lexing started
	Token current_token;              // The current token being processed
} Lexer;

Lexer lexer_create(File* source_file);

void lexer_skip_whitespace(Lexer* lexer);

bool lexer_try_get_next_token(Lexer* lexer);
