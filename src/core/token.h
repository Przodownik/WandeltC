/**
 * @file token.h
 * @author TF
 * @copyright Copyright (c) 2025 TF
 */
#pragma once

// When adding a new token type:
// 1) Add it to the switch inside token_type_to_string method.
typedef enum _TokenType
{
	// keywords
	TOKEN_FUNCTION_KEYWORD = 0, // fn
	TOKEN_RETURN_KEYWORD,       // return
	TOKEN_INT32_KEYWORD,        // int32

	// single character tokens
	TOKEN_OPEN_PAREN,    // (
	TOKEN_CLOSE_PAREN,   // )
	TOKEN_OPEN_BRACE,    // {
	TOKEN_CLOSE_BRACE,   // }
	TOKEN_OPEN_BRACKET,  // [
	TOKEN_CLOSE_BRACKET, // ]
	TOKEN_PLUS,          // +
	TOKEN_MINUS,         // -
	TOKEN_STAR,          // *
	TOKEN_SLASH,         // /
	TOKEN_PERCENT,       // %
	TOKEN_EQUAL,         // =
	TOKEN_EXCLAMATION,   // !
	TOKEN_LESS,          // <
	TOKEN_GREATER,       // >
	TOKEN_COMMA,         // ,
	TOKEN_DOT,           // .
	TOKEN_COLON,         // :
	TOKEN_SEMICOLON,     // ;
	TOKEN_APOSTROPHE,    // '
	TOKEN_QUOTE,         // "

	// double character tokens
	TOKEN_STAR_STAR,        // **
	TOKEN_EQUAL_EQUAL,      // ==
	TOKEN_NOT_EQUAL,        // !=
	TOKEN_LESS_OR_EQUAL,    // <=
	TOKEN_GREATER_OR_EQUAL, // >=
	TOKEN_ARROW,            // ->
	TOKEN_COLON_COLON,      // ::

	// other
	TOKEN_IDENTIFIER, // e.g. somename1
	TOKEN_NUMBER,     // 12
	TOKEN_STRING,     // "string"
	TOKEN_CHARACTER,  // 'c'
	TOKEN_UNKNOWN,
	TOKEN_EOF,
} TokenType;

const char* token_type_to_string(TokenType type);
