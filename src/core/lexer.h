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

/**
 * @brief Creates a new lexer instance and sets the lexer properties.
 * @param source_file The source file to be lexed.
 * @return A new lexer instance.
 */
Lexer lexer_create(File* source_file);

/**
 * @brief Starts a new token from the current position in the lexer.
 * @param lexer The lexer instance.
 */
void lexer_start_new_token(Lexer* lexer);

/**
 * @brief Advances the lexer to the next character.
 * @param lexer The lexer instance.
 */
void lexer_advance(Lexer* lexer);

/**
 * @brief Advances the lexer and returns the current character.
 * @param lexer The lexer instance.
 * @return The current character after advancing.
 */
char lexer_advance_and_get(Lexer* lexer);

/**
 * @brief Gets the current character and advances the lexer.
 * @param lexer The lexer instance.
 * @return The current character before advancing.
 */
char lexer_get_and_advance(Lexer* lexer);

/**
 * @brief Skips the next character(s) in the lexer.
 * @param lexer The lexer instance.
 * @param amount The number of characters to skip.
 */
void lexer_skip_next(Lexer* lexer, uint32 amount);

/**
 * @brief Checks if the current character matches the expected character.
 * @param lexer The lexer instance.
 * @param expected The expected character.
 * @return True if the current character matches the expected character, false otherwise.
 */
bool lexer_match(Lexer* lexer, char expected);

/**
 * @brief Checks if the next character matches the expected character.
 * @param lexer The lexer instance.
 * @param expected The expected character.
 * @return True if the next character matches the expected character, false otherwise.
 */
bool lexer_match_next(Lexer* lexer, char expected);

/**
 * @brief Checks if the current character matches the expected character and advances the lexer.
 * @param lexer The lexer instance.
 * @param expected The expected character.
 * @return True if the current character matches the expected character, false otherwise.
 */
bool lexer_match_and_advance(Lexer* lexer, char expected);

/**
 * @brief Skips whitespace characters in the lexer.
 * @param lexer The lexer instance.
 */
void lexer_skip_whitespace(Lexer* lexer);

/**
 * @brief Creates a new token and sets its properties based in lexer state.
 * @param lexer The lexer instance.
 * @param type The type of the token to create.
 * @param lexeme The lexeme of the token to create.
 * @return True if the token was created successfully, false otherwise.
 */
bool lexer_make_new_token(Lexer* lexer, TokenType type, const char* lexeme);

/**
 * @brief Creates a new error token covering span from the start of the lexer to the current position.
 * @param lexer The lexer instance.
 * @param msg The error message to set.
 * @param ... Additional arguments for the error message.
 * @return True if the error token was created successfully, false otherwise.
 */
bool lexer_make_new_verror_token_at_lexing_start(Lexer* lexer, const char* msg, ...);

/**
 * @brief Consumes the digit(s) in the lexer and creates a new token with the consumed digits. (Also deduces the type of
 *		  the number e.g. integer / float)
 * @param lexer The lexer instance.
 * @return True if the token was created successfully, false otherwise.
 */
bool lexer_scan_digit(Lexer* lexer);

/**
 * @brief Tries to get the next token from the lexer.
 * @warning Lexer internal use only, use lexer_try_get_next_token instead!
 * @param lexer The lexer instance.
 * @return True if the next token was retrieved successfully, false otherwise.
 */
bool _lexer_try_get_next_token(Lexer* lexer);

/**
 * @brief Tries to get the next token from the lexer.
 *		  If error token occurred it scans all further tokens to check for other errors.
 * @param lexer The lexer instance.
 * @return True if the next token was retrieved successfully, false otherwise.
 */
bool lexer_try_get_next_token(Lexer* lexer);
