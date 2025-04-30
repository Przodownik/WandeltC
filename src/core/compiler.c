#include "compiler.h"

#include "codegen.h"
#include "lexer.h"
#include "parser.h"
#include "semantic_analyser.h"
#include "utils/defines.h"

extern Context global_context; // from compiler_internal.h

void compiler_create(Compiler* compiler)
{
	compiler->build_options.project_name     = nullptr;
	compiler->build_options.deliverable_type = DELIVERABLE_TYPE_EXECUTABLE;
	compiler->build_options.file_sources     = nullptr;
	compiler->build_options.lexer_debug      = false;
	compiler->build_options.parsed_debug     = false;
}

void compiler_lex(Compiler* compiler)
{
	for (uint64 i = 0; i < vector_get_length(compiler->build_options.file_sources); ++i)
	{
		TRACE(ANSI_COLOR_CYAN "Lexing file %s...\n" ANSI_COLOR_RESET, compiler->build_options.file_sources[i].path);

		Lexer lexer = lexer_create(&compiler->build_options.file_sources[i]);

		Clock clock = clock_create();

		while (lexer_try_get_next_token(&lexer))
		{
			Token token = lexer.current_token;

			if (compiler->build_options.lexer_debug)
			{
				TRACE("<Token id=\"%i\" type=\"%s\", value=\"%s\" />\n", lexer.token_count,
				      token_type_to_enum_stringified(token.type),
				      (token.type == TOKEN_IDENTIFIER || token.type == TOKEN_NUMBER || token.type == TOKEN_STRING)
				          ? token.lexeme
				          : token_type_to_string(token.type));
			}
		}

		TRACE(ANSI_COLOR_CYAN "Lexing file %s took %f ms\n" ANSI_COLOR_RESET,
		      compiler->build_options.file_sources[i].path, clock_get_elapsed_time(&clock) * 1000.0f);
	}
}

void compiler_parse(Compiler* compiler)
{
	for (uint64 i = 0; i < vector_get_length(compiler->build_options.file_sources); ++i)
	{
		TRACE(ANSI_COLOR_CYAN "Lexing and parsing file %s...\n" ANSI_COLOR_RESET,
		      compiler->build_options.file_sources[i].path);

		Lexer lexer = lexer_create(&compiler->build_options.file_sources[i]);

		Clock clock = clock_create();

		Parser parser = parser_create(&global_context, &lexer);
		parser_parse(&parser);

		TRACE(ANSI_COLOR_CYAN "Lexing and parsing file %s took %f ms\n" ANSI_COLOR_RESET,
		      compiler->build_options.file_sources[i].path, clock_get_elapsed_time(&clock) * 1000.0f);
	}

	global_context_emit_json(&global_context);
}

void compiler_compile(Compiler* compiler)
{
	compiler_parse(compiler);

	sema_analyse_parsed_context();

	if (global_context.error_count > 0)
	{
		TRACE(ANSI_COLOR_RED "Stopping codegen because of %i errors...\n" ANSI_COLOR_RESET, global_context.error_count);
		return;
	}

	codegen_generate();

	if (global_context.warning_count > 0)
	{
		TRACE(ANSI_COLOR_ORANGE "Compilation finished with %i warnings\n" ANSI_COLOR_RESET,
		      global_context.warning_count);
		return;
	}

	TRACE(ANSI_COLOR_GREEN "Compilation finished successfully\n" ANSI_COLOR_RESET);
}
