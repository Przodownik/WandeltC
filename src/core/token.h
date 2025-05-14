/**
 * @file token.h
 * @author TF
 * @copyright Copyright (c) 2025 TF
 */
#pragma once

// When adding a new token type:
// 1) Add it to the switch inside token_type_to_string method.
// 2) Add it to the switch inside token_type_to_enum_stringified method.
// 3) If the token is a keyword add it to the symbol table inside compiler_internal_initialize method in
//	  compiler_internal.h
// 4) If it is also a type consider proper casts support.
typedef enum _TokenType
{
	// keywords
	TOKEN_FUNCTION_KEYWORD = 0, // fn
	TOKEN_RETURN_KEYWORD,       // return
	TOKEN_TRUE_KEYWORD,         // true
	TOKEN_FALSE_KEYWORD,        // false
	TOKEN_PUBLIC_KEYWORD,       // public
	TOKEN_PRIVATE_KEYWORD,      // private
	TOKEN_IF_KEYWORD,           // if
	TOKEN_ELSE_KEYWORD,         // else

	// builtin types
	TOKEN_VOID_KEYWORD,   // void
	TOKEN_BOOL_KEYWORD,   // bool
	TOKEN_CHAR_KEYWORD,   // char
	TOKEN_UCHAR_KEYWORD,  // uchar
	TOKEN_SHORT_KEYWORD,  // short
	TOKEN_USHORT_KEYWORD, // ushort
	TOKEN_INT_KEYWORD,    // int
	TOKEN_UINT_KEYWORD,   // uint
	TOKEN_LONG_KEYWORD,   // long
	TOKEN_ULONG_KEYWORD,  // ulong
	TOKEN_FLOAT_KEYWORD,  // float
	TOKEN_DOUBLE_KEYWORD, // double

	TOKEN_KEYWORD_COUNT,

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
	TOKEN_INTEGER,    // e.g. 12
	TOKEN_FLOAT,      // e.g. 12.34f
	TOKEN_DOUBLE,     // e.g. 12.34d
	TOKEN_STRING,     // e.g."string"
	TOKEN_CHARACTER,  // e.g.'c'
	TOKEN_UNKNOWN,
	TOKEN_EOF,
} TokenType;

/**
 * @brief Converts a TokenType to a string representation. e.g. uint
 *
 * @param type The TokenType to convert.
 * @return A string representation of the TokenType.
 */
const char* token_type_to_string(TokenType type);

/**
 * @brief Converts a TokenType to a string representation for enum. e.g. TOKEN_UINT_KEYWORD
 *
 * @param type The TokenType to convert.
 * @return A string representation of the TokenType for enum.
 */
const char* token_type_to_enum_stringified(TokenType type);

/**
 * @brief Checks if the given token type is a builtin type. e.g. int, float, etc.
 *
 * @param type The TokenType to check.
 * @return true if the token type is a keyword, false otherwise.
 */
bool is_token_type_a_type_token(TokenType type);
