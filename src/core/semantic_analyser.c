#include "semantic_analyser.h"
#include "diagnostics.h"

extern HashMap type_table; // from compiler_internal.h

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

bool sema_analyse_statement(SemaContext* sema_context, Statement* statement);

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

Declaration* sema_try_get_defined_function(SemaContext* sema_context, const char* name)
{
	for (uint64 i = 0; i < vector_get_length(sema_context->analysed_functions); ++i)
	{
		Declaration* function_declaration = sema_context->analysed_functions[i];
		if (strcmp(function_declaration->function.signature.name, name) == 0)
		{
			return function_declaration;
		}
	}

	return nullptr;
}

bool sema_analyse_compound_statement(SemaContext* sema_context, Statement* statement)
{
	sema_push_scope(sema_context, statement->compound.first);

	Statement* current = statement->compound.first;

	while (current != nullptr)
	{
		if (!sema_analyse_statement(sema_context, current))
			return false;

		current = current->next;
	}

	sema_pop_scope(sema_context);

	return true;
}

Declaration* sema_resolve_identifier_expression(SemaContext* sema_context, Expression* expression)
{
	for (int32 i = 0; i < sema_context->current_scope_depth; ++i)
	{
		Scope* current_scope = &sema_context->scopes[i];

		Statement* first = current_scope->first_scope_statement;

		while (first != nullptr)
		{
			if (first->kind == STATEMENT_DECLARATION)
			{
				Declaration* declaration = first->declaration.declaration;

				if (declaration->kind == DECLARATION_VARIABLE &&
				    strcmp(declaration->variable.name, expression->identifier.name) == 0)
				{
					if (declaration->resolve_status == RESOLVE_STATUS_RESOLVING)
					{
						sema_report_error(
						    &expression->source_span,
						    "Variable '%s' is being resolved recursively. Please check for circular dependencies.",
						    expression->identifier.name);
						return nullptr;
					}

					return declaration;
				}
			}

			first = first->next;
		}
	}

	sema_report_error(&expression->source_span,
	                  "Could not resolve identifier '%s'. Please check if it is declared in the current scope.",
	                  expression->identifier.name);

	return nullptr;
}

Type* sema_deduce_type_for_expression(SemaContext* sema_context, Expression* expression)
{
	switch (expression->kind)
	{
	case EXPRESSION_CONSTANT:
		// Already set by the parser
		break;
	case EXPRESSION_BINARY:
		Type* left_type  = sema_deduce_type_for_expression(sema_context, expression->binary.left);
		Type* right_type = sema_deduce_type_for_expression(sema_context, expression->binary.right);

		if (left_type != right_type)
		{
			sema_report_error(&expression->source_span,
			                  "Binary operator '%s' cannot be applied to different types '%s' and '%s'. Unless you are "
			                  "doing a cast operation, please check the types of the operands.",
			                  binary_operator_to_string(expression->binary.operator),
			                  type_kind_to_string(left_type->kind), type_kind_to_string(right_type->kind));

			return nullptr;
		}

		switch (expression->binary.operator)
		{
		LOGICAL_OPERATORS:
			Type** bool_type = (Type**)hash_map_get_value(&type_table, token_type_to_string(TOKEN_BOOL_KEYWORD));
			expression->type = *bool_type;
			break;
		default:
			expression->type = left_type;
			break;
		}
		break;

	case EXPRESSION_UNARY:
		expression->type = sema_deduce_type_for_expression(sema_context, expression->unary.operand);
		break;
	case EXPRESSION_GROUP:
		expression->type = sema_deduce_type_for_expression(sema_context, expression->group.expression);
		break;
	case EXPRESSION_IDENTIFIER:
		expression->type = expression->identifier.refered->variable.type;
		break;
	case EXPRESSION_CAST:
		// Already set by the parser
		break;
	default:
		ASSERT(false, "Invalid expression kind: %d\n", expression->kind);
		break;
	}

	return expression->type;
}

bool sema_analyse_expression(SemaContext* sema_context, Expression* expression);

CastKind sema_resolve_cast_kind(Type* cast_to, Type* expression_type)
{
	if (cast_to->kind == TYPE_KIND_BOOL && expression_type->kind == TYPE_KIND_INT)
		return CAST_INT32_TO_BOOL;
	else if (cast_to->kind == TYPE_KIND_INT && expression_type->kind == TYPE_KIND_BOOL)
		return CAST_BOOL_TO_INT32;

	return CAST_INVALID;
}

bool sema_analyse_expression_internal(SemaContext* sema_context, Expression* expression)
{
	switch (expression->kind)
	{
	case EXPRESSION_BINARY:
		if (!sema_analyse_expression(sema_context, expression->binary.left))
			return false;
		if (!sema_analyse_expression(sema_context, expression->binary.right))
			return false;
		break;
	case EXPRESSION_UNARY:
		if (!sema_analyse_expression(sema_context, expression->unary.operand))
			return false;
		break;
	case EXPRESSION_IDENTIFIER:
		expression->identifier.refered = sema_resolve_identifier_expression(sema_context, expression);
		if (expression->identifier.refered == nullptr)
			return false;
		break;
	case EXPRESSION_GROUP:
		if (!sema_analyse_expression(sema_context, expression->group.expression))
			return false;
		break;
	case EXPRESSION_CONSTANT:
		break;
	case EXPRESSION_CAST:
		if (!sema_analyse_expression(sema_context, expression->cast.expression))
			return false;

		expression->cast.cast_kind =
		    sema_resolve_cast_kind(expression->cast.cast_to, expression->cast.expression->type);
		break;
	default:
		ASSERT(false, "Invalid expression kind: %d\n", expression->kind);
		break;
	}

	expression->resolve_status = RESOLVE_STATUS_RESOLVED;

	return sema_deduce_type_for_expression(sema_context, expression) != nullptr;
}

bool sema_analyse_expression(SemaContext* sema_context, Expression* expression)
{
	switch (expression->resolve_status)
	{
	case RESOLVE_STATUS_UNRESOLVED:
		expression->resolve_status = RESOLVE_STATUS_RESOLVING;
		return sema_analyse_expression_internal(sema_context, expression);
	case RESOLVE_STATUS_RESOLVING:
		sema_report_error(&expression->source_span,
		                  "Expression '%s' is being resolved recursively. Please check for circular dependencies.",
		                  expression->identifier.name);
		return false;
	case RESOLVE_STATUS_RESOLVED:
		return true;
	default:
		break;
	}

	UNREACHABLE;
}

bool sema_analyse_return_statement(SemaContext* sema_context, Statement* statement)
{
	if (statement->return_.expression != nullptr)
		return sema_analyse_expression(sema_context, statement->return_.expression);

	return true;
}

bool sema_analyse_statement(SemaContext* sema_context, Statement* statement)
{
	switch (statement->kind)
	{
	case STATEMENT_COMPOUND:
		return sema_analyse_compound_statement(sema_context, statement);
	case STATEMENT_RETURN:
		return sema_analyse_return_statement(sema_context, statement);
	case STATEMENT_DECLARATION:
		switch (statement->declaration.declaration->kind)
		{
		case DECLARATION_VARIABLE:
			statement->declaration.declaration->resolve_status = RESOLVE_STATUS_RESOLVING;

			if (!sema_analyse_expression(sema_context, statement->declaration.declaration->variable.initializer))
				return false;

			Type* variable_type    = statement->declaration.declaration->variable.type;
			Type* initializer_type = statement->declaration.declaration->variable.initializer->type;

			if (variable_type != initializer_type)
			{
				sema_report_error(&statement->source_span,
				                  "Variable '%s' of type '%s' cannot be initialized with expression of type '%s'.",
				                  statement->declaration.declaration->variable.name,
				                  type_kind_to_string(variable_type->kind),
				                  type_kind_to_string(initializer_type->kind));
				return false;
			}

			statement->declaration.declaration->resolve_status = RESOLVE_STATUS_RESOLVED;

			return true;
		case DECLARATION_FUNCTION:
			ASSERT(false, "Not top level function declarations are not supported.\n");
			break;
		default:
			break;
		}
		break;

	case STATEMENT_EXPRESSION:
		return sema_analyse_expression(sema_context, statement->expression.expression);
	default:
		ASSERT(false, "Invalid statement kind: %d\n", statement->kind);
		break;
	}

	return false;
}

bool sema_analyse_function_declaration(SemaContext* sema_context, Declaration* declaration)
{
	Declaration* existing_function = sema_try_get_defined_function(sema_context, declaration->function.signature.name);
	if (existing_function != nullptr)
	{
		sema_report_warning(&declaration->function.signature.source_span, "Function '%s' is being redefined!",
		                    declaration->function.signature.name);
		sema_report_error(&existing_function->function.signature.source_span,
		                  "Function '%s' was already declared here!", declaration->function.signature.name);
		return false;
	}

	sema_push_scope(sema_context, declaration->function.body);

	bool result = sema_analyse_compound_statement(sema_context, declaration->function.body);

	sema_pop_scope(sema_context);

	// TODO: verify that function return type matches those with the return statements, or not require return if void

	Statement* function_body    = declaration->function.body;
	declaration->resolve_status = RESOLVE_STATUS_RESOLVED;

	vector_push(sema_context->analysed_functions, declaration);

	return result;
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
		Declaration* main_function = sema_try_get_defined_function(&sema_context, "main");

		if (main_function == nullptr)
		{
			ERROR("No main function found, please define a main function!\n");

			context->error_count++;
		}
		else
		{
			if (main_function->function.signature.return_type->kind != TYPE_KIND_INT)
			{
				sema_report_warning(&main_function->function.signature.source_span,
				                    "Main function should return '" YHOT("int") "', but it returns '" YHOT("%s") "'",
				                    type_kind_to_string(main_function->function.signature.return_type->kind));
			}
		}
	}

	vector_destroy(sema_context.analysed_functions);

	TRACE(ANSI_COLOR_CYAN "Semantic analysis took %f ms\n" ANSI_COLOR_RESET, clock_get_elapsed_time(&clock) * 1000.0f);
}
