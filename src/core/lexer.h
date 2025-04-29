/**
 * @file lexer.h
 * @author TF
 * @copyright Copyright (c) 2025 TF
 */
#pragma once

#include "core/compiler_internal.h"

typedef struct _Lexer
{
	const File* source_file;     // The source file to be lexed
	const char* lexing_start;    // The start of the current token being lexed
	const char* current_char;    // The current character being lexed
	const char* line_start_char; // The start of the current line being lexed
	uint32 current_row_number;   // The current row number in the file being lexed
	Token current_token;         // The current token being processed
	Token previous_token;        // The previously processed token (for error recovery)
	uint32 token_count;          // Total amount of lexed tokens
} Lexer;

Lexer lexer_create(File* source_file);

void lexer_start_new_token(Lexer* lexer);

void lexer_advance(Lexer* lexer);

char lexer_advance_and_get(Lexer* lexer);

char lexer_get_and_advance(Lexer* lexer);

void lexer_skip_next(Lexer* lexer, uint32 amount);

bool lexer_match(Lexer* lexer, char expected);

bool lexer_match_next(Lexer* lexer, char expected);

bool lexer_match_and_advance(Lexer* lexer, char expected);

void lexer_skip_whitespace(Lexer* lexer);

bool lexer_make_new_token(Lexer* lexer, TokenType type, char* lexeme);

bool lexer_make_new_verror_token_at_lexing_start(Lexer* lexer, const char* msg, ...);

bool lexer_scan_digit(Lexer* lexer);

bool _lexer_try_get_next_token(Lexer* lexer);

bool lexer_try_get_next_token(Lexer* lexer);
