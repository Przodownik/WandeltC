#include "compiler.h"

#include "lexer.h"

void compiler_create(Compiler* compiler)
{
	compiler->build_options.project_name     = nullptr;
	compiler->build_options.deliverable_type = DELIVERABLE_TYPE_EXECUTABLE;
	compiler->build_options.file_sources     = nullptr;
}

void compiler_lex(Compiler* compiler)
{
	for (uint64 i = 0; i < vector_get_length(compiler->build_options.file_sources); ++i)
	{
		Lexer lexer = lexer_create(&compiler->build_options.file_sources[i]);

		while (lexer_try_get_next_token(&lexer))
		{
			Token token = lexer.current_token;

			TRACE("<Token type=\"%s\", value=\"%s\" />\n", token_type_to_enum_stringified(token.type),
			      (token.type == TOKEN_IDENTIFIER || token.type == TOKEN_NUMBER || token.type == TOKEN_STRING)
			          ? token.lexeme
			          : token_type_to_string(token.type));
		}
	}
}

void compiler_parse(Compiler* compiler)
{
}

void compiler_compile(Compiler* compiler)
{
	compiler_lex(compiler);
	compiler_parse(compiler);
}
