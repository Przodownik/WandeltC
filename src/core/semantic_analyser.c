#include "semantic_analyser.h"

extern Context global_context; // from compiler_internal.h

void sema_report_error(SourceSpan* location, const char* message, ...)
{
	global_context.error_count++;

	va_list list;
	va_start(list, message);
	diagnostics_verror_along_span(location, message, list);
	va_end(list);
}

void sema_report_warning(SourceSpan* location, const char* message, ...)
{
	global_context.warning_count++;

	va_list list;
	va_start(list, message);
	diagnostics_vwarning_along_span(location, message, list);
	va_end(list);
}

void sema_analyse_function_declaration(Declaration* declaration)
{
	const FunctionSignature signature = declaration->function.signature;
	const Statement* first_body_statement  = declaration->function.body->compound.first;

	// check if return type is valid
	while (first_body_statement != nullptr)
	{
		if (first_body_statement->type == STATEMENT_RETURN)
		{
			ERROR("Invalid return type.");
		}
		
		first_body_statement = first_body_statement->next;
	}
}

void sema_analyse_parsed_context(void)
{
	TRACE(ANSI_COLOR_CYAN "Semantic analysis file...\n" ANSI_COLOR_RESET);

	Clock clock = clock_create();

	for (uint64 i = 0; i < vector_get_length(global_context.functions_declarations); ++i)
	{
		Declaration* function_declaration = global_context.functions_declarations[i];
	}

	TRACE(ANSI_COLOR_CYAN "Semantic analysis took %f ms\n" ANSI_COLOR_RESET, clock_get_elapsed_time(&clock) * 1000.0f);
}
