#include "lexer.h"

#include "diagnostics.h"
#include "utils/defines.h"

extern ArenaAllocator string_allocator; // from compiler_internal.h
extern HashMap symbol_table;            // from compiler_internal.h
extern Context global_context;          // from compiler_internal.h

#define lexer_get_current_char(lexer)  (*(lexer)->current_char)
#define lexer_get_previous_char(lexer) (*((lexer)->current_char - 1))
#define lexer_get_next_char(lexer)     (*((lexer)->current_char + 1))
#define lexer_is_eof(lexer)            (*(lexer)->current_char == '\0')
#define lexer_is_at_newline(lexer)     (*(lexer)->current_char == '\n')

#define is_character_a_digit(c)         c >= '0' && c <= '9'
#define is_character_an_alphanumeric(c) (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')

Lexer lexer_create(File* source_file)
{
	Lexer lexer              = {0};
	lexer.source_file        = source_file;
	lexer.lexing_start       = lexer.source_file->content;
	lexer.current_char       = lexer.source_file->content;
	lexer.line_start_char    = lexer.source_file->content;
	lexer.current_row_number = 1u;
	lexer.token_count        = 0u;

	return lexer;
}

void lexer_start_new_token(Lexer* lexer)
{
	lexer->lexing_start = lexer->current_char;
}

void lexer_advance(Lexer* lexer)
{
	if (lexer_is_eof(lexer))
		return;

	if (lexer_is_at_newline(lexer))
	{
		lexer->line_start_char = lexer->current_char + 1;
		lexer->current_row_number++;
	}

	lexer->current_char++;
}

char lexer_advance_and_get(Lexer* lexer)
{
	lexer_advance(lexer);

	return lexer_get_current_char(lexer);
}

char lexer_get_and_advance(Lexer* lexer)
{
	char current = lexer_get_current_char(lexer);
	lexer_advance(lexer);

	return current;
}

void lexer_skip_next(Lexer* lexer, uint32 amount)
{
	ASSERT(amount > 0, "Amount must be positive");

	for (uint32 i = 0; i < amount; i++) lexer_advance(lexer);
}

bool lexer_match(Lexer* lexer, char expected)
{
	if (lexer_is_eof(lexer))
		return false;

	return lexer_get_current_char(lexer) == expected;
}

bool lexer_match_next(Lexer* lexer, char expected)
{
	if (lexer_is_eof(lexer))
		return false;

	return lexer_get_next_char(lexer) == expected;
}

bool lexer_match_and_advance(Lexer* lexer, char expected)
{
	if (!lexer_match(lexer, expected))
		return false;

	lexer_advance(lexer);

	return true;
}

void lexer_skip_whitespace(Lexer* lexer)
{
	while (true)
	{
		switch (lexer_get_current_char(lexer))
		{
		case ' ':
		case '\r':
		case '\t':
		case '\n':
			lexer_advance(lexer);
			break;
		case '/':
			if (lexer_get_next_char(lexer) == '/')
			{
				lexer_skip_next(lexer, 2); // skip '//'

				// skip until the end of the line
				while (!lexer_is_eof(lexer) && lexer_get_current_char(lexer) != '\n') lexer_advance(lexer);

				if (lexer_get_current_char(lexer) == '\n')
					lexer_advance(lexer);

				continue;
			}

			if (lexer_get_next_char(lexer) == '*')
			{
				lexer_skip_next(lexer, 2); // skip '*/'

				while (true)
				{
					lexer_advance(lexer);

					if (lexer_match(lexer, '*') && lexer_match_next(lexer, '/'))
					{
						lexer_skip_next(lexer, 2); // skip '*/'
						break;
					}

					if (lexer_is_eof(lexer))
					{
						break;
					}
				}

				continue;
			}

			return;
		default:
			return;
		}
	}
}

bool lexer_make_new_token(Lexer* lexer, TokenType type, const char* lexeme)
{
	SourceSpan span;
	span.source_file = lexer->source_file;
	span.row         = lexer->current_row_number;
	span.column =
	    get_display_column(lexer->line_start_char, (uint32)(lexer->lexing_start - lexer->line_start_char) + 1);
	span.length = (uint32)(lexer->current_char - lexer->lexing_start);

	Token token;
	token.lexeme      = lexeme;
	token.type        = type;
	token.source_span = span;

	lexer->previous_token = lexer->current_token;
	lexer->current_token  = token;
	lexer->token_count++;

	return true;
}

bool lexer_make_new_verror_token_at_lexing_start(Lexer* lexer, const char* msg, ...)
{
	SourceSpan span;
	span.source_file = lexer->source_file;
	span.row         = lexer->current_row_number;
	span.column =
	    get_display_column(lexer->line_start_char, (uint32)(lexer->lexing_start - lexer->line_start_char) + 1);
	span.length      = 1;

	va_list list;
	va_start(list, msg);
	diagnostics_verror_along_span(&span, msg, list);
	va_end(list);

	global_context.error_count++;

	return false;
}

bool lexer_scan_digit(Lexer* lexer)
{
	while (is_character_a_digit(lexer_get_current_char(lexer))) lexer_advance(lexer);

	if (lexer_match(lexer, '.'))
	{
		lexer_advance(lexer);

		while (is_character_a_digit(lexer_get_current_char(lexer)))
		{
			lexer_advance(lexer);
		}
	}

	uint32 lexeme_length = (uint32)(lexer->current_char - lexer->lexing_start);

	char* buffer = arena_allocator_allocate(&string_allocator, lexeme_length + 1);

	return lexer_make_new_token(lexer, TOKEN_NUMBER,
	                            cstring_copy_part_into_buffer(lexer->lexing_start, lexeme_length, buffer));
}

bool _lexer_try_get_next_token(Lexer* lexer)
{
	lexer_skip_whitespace(lexer);
	lexer_start_new_token(lexer);

	if (lexer_is_eof(lexer))
	{
		lexer_make_new_token(lexer, TOKEN_EOF, "EOF");
		lexer->current_token.source_span.length = 1;

		return false;
	}

	const char current = lexer_get_and_advance(lexer);

	switch (current)
	{
	case '(':
		return lexer_make_new_token(lexer, TOKEN_OPEN_PAREN, "(");
	case ')':
		return lexer_make_new_token(lexer, TOKEN_CLOSE_PAREN, ")");
	case '{':
		return lexer_make_new_token(lexer, TOKEN_OPEN_BRACE, "{");
	case '}':
		return lexer_make_new_token(lexer, TOKEN_CLOSE_BRACE, "}");
	case '[':
		return lexer_make_new_token(lexer, TOKEN_OPEN_BRACKET, "[");
	case ']':
		return lexer_make_new_token(lexer, TOKEN_CLOSE_BRACKET, "]");
	case '+':
		return lexer_make_new_token(lexer, TOKEN_PLUS, "+");
	case '-':
		if (lexer_match(lexer, '>'))
		{
			lexer_advance(lexer);
			return lexer_make_new_token(lexer, TOKEN_ARROW, "->");
		}

		return lexer_make_new_token(lexer, TOKEN_MINUS, "-");
	case '*':
		if (lexer_match(lexer, '*'))
		{
			lexer_advance(lexer);
			return lexer_make_new_token(lexer, TOKEN_STAR_STAR, "**");
		}

		return lexer_make_new_token(lexer, TOKEN_STAR, "*");
	case '/':
		return lexer_make_new_token(lexer, TOKEN_SLASH, "/");
	case '%':
		return lexer_make_new_token(lexer, TOKEN_PERCENT, "%");
	case '=':
		if (lexer_match(lexer, '='))
		{
			lexer_advance(lexer);
			return lexer_make_new_token(lexer, TOKEN_EQUAL_EQUAL, "==");
		}

		return lexer_make_new_token(lexer, TOKEN_EQUAL, "=");
	case '!':
		if (lexer_match(lexer, '='))
		{
			lexer_advance(lexer);
			return lexer_make_new_token(lexer, TOKEN_NOT_EQUAL, "!=");
		}

		return lexer_make_new_token(lexer, TOKEN_EXCLAMATION, "!");
	case '<':
		if (lexer_match(lexer, '='))
		{
			lexer_advance(lexer);
			return lexer_make_new_token(lexer, TOKEN_LESS_OR_EQUAL, "<=");
		}

		return lexer_make_new_token(lexer, TOKEN_LESS, "<");
	case '>':
		if (lexer_match(lexer, '='))
		{
			lexer_advance(lexer);
			return lexer_make_new_token(lexer, TOKEN_GREATER_OR_EQUAL, ">=");
		}

		return lexer_make_new_token(lexer, TOKEN_GREATER, ">");
	case ',':
		return lexer_make_new_token(lexer, TOKEN_COMMA, ",");
	case '.':
		return lexer_make_new_token(lexer, TOKEN_DOT, ".");
	case ':':
		if (lexer_match(lexer, ':'))
		{
			lexer_advance(lexer);
			return lexer_make_new_token(lexer, TOKEN_COLON_COLON, "::");
		}

		return lexer_make_new_token(lexer, TOKEN_COLON, ":");
	case ';':
		return lexer_make_new_token(lexer, TOKEN_SEMICOLON, ";");
	default:
		if (is_character_a_digit(current))
		{
			return lexer_scan_digit(lexer);
		}
		else if (is_character_an_alphanumeric(current))
		{
			char next = lexer_get_current_char(lexer);

			while (is_character_an_alphanumeric(next) || is_character_a_digit(next))
				next = lexer_advance_and_get(lexer);

			uint32 lexeme_length   = (uint32)(lexer->current_char - lexer->lexing_start);
			const CStringView view = {lexer->lexing_start, lexeme_length};

			TokenType* token_type = hash_map_get_value_by_view(&symbol_table, &view);
			if (token_type)
			{
				return lexer_make_new_token(lexer, *token_type, token_type_to_string(*token_type));
			}

			char* buffer = arena_allocator_allocate(&string_allocator, lexeme_length + 1);

			return lexer_make_new_token(lexer, TOKEN_IDENTIFIER,
			                            cstring_copy_part_into_buffer(lexer->lexing_start, lexeme_length, buffer));
		}

		return lexer_make_new_verror_token_at_lexing_start(
		    lexer, "Unexpected character '" ANSI_COLOR_YELLOW "%c" ANSI_COLOR_RED "' found!" ANSI_COLOR_RESET, current);
	}
}

bool lexer_try_get_next_token(Lexer* lexer)
{
	if (_lexer_try_get_next_token(lexer))
		return true;

	if (lexer_is_eof(lexer))
		return false;

	bool is_token_valid = false;

	// scan for other invalid tokens
	do
	{
		if (!is_token_valid) // one invalid token per line is evaluated to reduce noise
		{
			while (!lexer_is_eof(lexer) && !lexer_is_at_newline(lexer)) lexer_advance(lexer);
		}

		is_token_valid = lexer_try_get_next_token(lexer);
	} while (!lexer_is_eof(lexer));

	return false;
}
