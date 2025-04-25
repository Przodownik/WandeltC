#include "lexer.h"

#define lexer_get_current_char(lexer)  (*(lexer)->current)
#define lexer_get_previous_char(lexer) (*((lexer)->current - 1))
#define lexer_get_next_char(lexer)     (*((lexer)->current + 1))
#define lexer_is_eof(lexer)            (*(lexer)->current == '\0')
#define lexer_is_at_newline(lexer)     (*(lexer)->current == '\n')

Lexer lexer_create(File* source_file)
{
	Lexer lexer           = {0};
	lexer.file_begin_char = source_file->content;

	return lexer;
}

void lexer_skip_whitespace(Lexer* lexer)
{
}

bool lexer_try_get_next_token(Lexer* lexer)
{
	lexer_skip_whitespace(lexer);

	return false;
}
