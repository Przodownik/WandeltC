#include "semantic_analyser.h"
#include "diagnostics.h"

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

#define MAX_SCOPE_DEPTH 16

typedef struct _Scope
{
	Statement* first_scope_statement; // traverse with ->next
} Scope;

typedef struct _SemaContext
{
	Scope* current_scope;
	int32 current_scope_depth;
	Scope scopes[MAX_SCOPE_DEPTH];
	Declaration** analysed_functions;
} SemaContext;

void sema_analyse_statement(SemaContext* sema_context, Statement* statement);

void sema_push_scope(SemaContext* sema_context, Statement* statement)
{
	VERIFY(sema_context->current_scope_depth < MAX_SCOPE_DEPTH, "Maximum scope depth exceeded");
	sema_context->current_scope                        = &sema_context->scopes[sema_context->current_scope_depth++];
	sema_context->current_scope->first_scope_statement = statement;
}

void sema_pop_scope(SemaContext* sema_context)
{
	VERIFY(sema_context->current_scope_depth > 0, "No scope to pop");
	sema_context->current_scope_depth--;
	sema_context->current_scope = &sema_context->scopes[sema_context->current_scope_depth];
}

bool sema_was_function_declared(SemaContext* sema_context, const char* name)
{
	for (uint64 i = 0; i < vector_get_length(sema_context->analysed_functions); ++i)
	{
		Declaration* function_declaration = sema_context->analysed_functions[i];
		if (strcmp(function_declaration->function.signature.name, name) == 0)
		{
			return true;
		}
	}

	return false;
}

void sema_analyse_compound_statement(SemaContext* sema_context, Statement* statement)
{
	sema_push_scope(sema_context, statement->compound.first);

	Statement* current = statement->compound.first;

	while (current != nullptr)
	{
		sema_analyse_statement(sema_context, current);
		current = current->next;
	}

	sema_pop_scope(sema_context);
}

Declaration* sema_resolve_identifier_expression(SemaContext* sema_context, Expression* expression)
{
	for (int32 i = 0; i < sema_context->current_scope_depth; ++i)
	{
		Scope* current_scope = &sema_context->scopes[i];

		Statement* first = current_scope->first_scope_statement;

		while (first != nullptr)
		{
			if (first->type == STATEMENT_DECLARATION)
			{
				Declaration* declaration = first->declaration.declaration;
				if (declaration->kind == DECLARATION_VARIABLE &&
				    strcmp(declaration->variable.name, expression->identifier.name) == 0)
				{
					return declaration;
				}
			}

			first = first->next;
		}
	}

	ASSERT(false, "Could not resolve identifier: %s\n", expression->identifier.name);
}

void sema_analyse_expression(SemaContext* sema_context, Expression* expression)
{
	switch (expression->kind)
	{
	case EXPRESSION_BINARY:
		sema_analyse_expression(sema_context, expression->binary.left);
		sema_analyse_expression(sema_context, expression->binary.right);
		break;
	case EXPRESSION_UNARY:
		sema_analyse_expression(sema_context, expression->unary.operand);
		break;
	case EXPRESSION_IDENTIFIER:
		expression->identifier.refered = sema_resolve_identifier_expression(sema_context, expression);
		break;
	case EXPRESSION_GROUP:
		sema_analyse_expression(sema_context, expression->group.expression);
		break;
	case EXPRESSION_LITERAL:
		break;
	default:
		break;
	}
}

void sema_analyse_return_statement(SemaContext* sema_context, Statement* statement)
{
	if (statement->return_.expression != nullptr)
		sema_analyse_expression(sema_context, statement->return_.expression);
}

void sema_analyse_statement(SemaContext* sema_context, Statement* statement)
{
	switch (statement->type)
	{
	case STATEMENT_COMPOUND:
		sema_analyse_compound_statement(sema_context, statement);
		break;
	case STATEMENT_RETURN:
		sema_analyse_return_statement(sema_context, statement);
		break;
	case STATEMENT_DECLARATION:
		switch (statement->declaration.declaration->kind)
		{
		case DECLARATION_VARIABLE:
			sema_analyse_expression(sema_context, statement->declaration.declaration->variable.initializer);
			break;
		case DECLARATION_FUNCTION:
			ASSERT(false, "Not top level function declarations are not supported.\n");
			break;
		default:
			break;
		}
		break;

	case STATEMENT_EXPRESSION:
		sema_analyse_expression(sema_context, statement->expression.expression);
		break;

	default:
		break;
	}
}

void sema_analyse_function_declaration(SemaContext* sema_context, Declaration* declaration)
{
	for (uint32 i = 0; i < vector_get_length(sema_context->analysed_functions); ++i)
	{
		Declaration* analysed_function = sema_context->analysed_functions[i];

		if (strcmp(analysed_function->function.signature.name, declaration->function.signature.name) == 0)
		{
			sema_report_warning(&declaration->function.signature.source_span, "Function '%s' is being redefined!",
			                    declaration->function.signature.name);

			sema_report_error(&analysed_function->function.signature.source_span,
			                  "Function '%s' was already declared here!", declaration->function.signature.name);
			return;
		}
	}

	sema_push_scope(sema_context, declaration->function.body);

	sema_analyse_compound_statement(sema_context, declaration->function.body);

	sema_pop_scope(sema_context);

	// TODO: verify that fiunction return type matches those with the return statements, or not require return if void

	Statement* function_body = declaration->function.body;

	vector_push(sema_context->analysed_functions, declaration);
}

void sema_analyse_parsed_context(Context* context)
{
	TRACE(ANSI_COLOR_CYAN "Semantic analysis file...\n" ANSI_COLOR_RESET);

	SemaContext sema_context        = {0};
	sema_context.analysed_functions = vector_create(5, sizeof(Declaration*));

	Clock clock = clock_create();

	for (uint64 i = 0; i < vector_get_length(context->functions_declarations); ++i)
	{
		Declaration* function_declaration = context->functions_declarations[i];

		sema_analyse_function_declaration(&sema_context, function_declaration);
	}

	// check if main function is defined
	// do not check it if there are any errors, because it could be a false positive
	if (context->error_count == 0)
	{
		if (!sema_was_function_declared(&sema_context, "main"))
		{
			ERROR("No main function found, please define a main function!\n");

			context->error_count++;
		}
	}

	vector_destroy(sema_context.analysed_functions);

	TRACE(ANSI_COLOR_CYAN "Semantic analysis took %f ms\n" ANSI_COLOR_RESET, clock_get_elapsed_time(&clock) * 1000.0f);
}
