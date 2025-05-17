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
	Declaration* current_function;
	int32 current_scope_depth;
	Scope scopes[MAX_SCOPE_DEPTH];
	HashMap defined_functions;
	HashMap analysed_functions;
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

Declaration* sema_try_get_function_from_map(HashMap* map, const char* name)
{
	Declaration** decl = (Declaration**)hash_map_get_value(map, name);
	if (decl != nullptr)
	{
		Declaration* function_declaration = *decl;

		if (function_declaration->resolve_status == RESOLVE_STATUS_RESOLVING)
		{
			sema_report_error(&function_declaration->source_span,
			                  "Function '%s' is being resolved recursively. Please check for circular dependencies.",
			                  function_declaration->function.signature.name);

			return nullptr;
		}

		return function_declaration;
	}

	return nullptr;
}

Declaration* sema_try_get_defined_function(SemaContext* sema_context, const char* name)
{
	return sema_try_get_function_from_map(&sema_context->defined_functions, name);
}

Declaration* sema_try_get_analysed_function(SemaContext* sema_context, const char* name)
{
	return sema_try_get_function_from_map(&sema_context->analysed_functions, name);
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
	// first check for parameter declarations
	if (sema_context->current_function->function.signature.parameters != nullptr)
	{
		for (uint64 i = 0; i < vector_get_length(sema_context->current_function->function.signature.parameters); ++i)
		{
			Declaration* parameter = sema_context->current_function->function.signature.parameters[i];
			if (strcmp(parameter->variable.name, expression->identifier.name) == 0)
			{
				if (parameter->resolve_status == RESOLVE_STATUS_RESOLVING)
				{
					sema_report_error(
					    &expression->source_span,
					    "Parameter '%s' is being resolved recursively. Please check for circular dependencies.",
					    expression->identifier.name);
					return nullptr;
				}

				return parameter;
			}
		}
	}

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
	case EXPRESSION_CALL:
		expression->type = sema_deduce_type_for_expression(sema_context, expression->call.callee);
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
	return compiler_internal_cast_table[expression_type->kind][cast_to->kind];
}

bool sema_analyse_call_expression(SemaContext* sema_context, Expression* expression)
{
	Declaration* function_declaration =
	    sema_try_get_defined_function(sema_context, expression->call.callee->identifier.name);

	if (function_declaration == nullptr)
	{
		sema_report_error(&expression->source_span,
		                  "Could not resolve function '%s'. Please check if it is declared in the current scope.",
		                  expression->call.callee->identifier.name);
		return false;
	}

	expression->call.callee->resolve_status     = RESOLVE_STATUS_RESOLVED;
	expression->call.callee->identifier.refered = function_declaration;

	if (function_declaration->function.signature.parameters == nullptr && expression->call.arguments != nullptr)
	{
		sema_report_error(&expression->source_span,
		                  "Function '%s' does not accept any arguments, but %d were provided.",
		                  function_declaration->function.signature.name, vector_get_length(expression->call.arguments));

		return false;
	}

	if (function_declaration->function.signature.parameters != nullptr &&
	    vector_get_length(function_declaration->function.signature.parameters) !=
	        vector_get_length(expression->call.arguments))
	{
		sema_report_error(&expression->source_span, "Function '%s' expects %d arguments, but %d were provided.",
		                  function_declaration->function.signature.name,
		                  vector_get_length(function_declaration->function.signature.parameters),
		                  vector_get_length(expression->call.arguments));

		return false;
	}

	if (expression->call.arguments != nullptr)
	{
		for (uint64 i = 0; i < vector_get_length(expression->call.arguments); ++i)
		{
			Expression* argument = expression->call.arguments[i];

			if (!sema_analyse_expression(sema_context, argument))
				return false;

			Type* parameter_type = function_declaration->function.signature.parameters[i]->variable.type;
			Type* argument_type  = argument->type;

			if (parameter_type != argument_type)
			{
				sema_report_error(
				    &argument->source_span,
				    "Argument %d of function '%s' of type '%s' cannot be passed as an argument of type '%s'.", i + 1,
				    function_declaration->function.signature.name, type_kind_to_string(parameter_type->kind),
				    type_kind_to_string(argument_type->kind));

				return false;
			}
		}
	}

	return true;
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

		if (expression->cast.cast_kind == CAST_INVALID)
		{
			sema_report_error(&expression->source_span, "Cannot cast from type '%s' to type '%s'.",
			                  type_kind_to_string(expression->cast.expression->type->kind),
			                  type_kind_to_string(expression->cast.cast_to->kind));
			return false;
		}

		if (expression->cast.cast_kind == CAST_SAME_TYPE)
		{
			sema_report_warning(&expression->source_span,
			                    "Casting from type '%s' to type '%s' is redundant. Please check the cast operation.",
			                    type_kind_to_string(expression->cast.expression->type->kind),
			                    type_kind_to_string(expression->cast.cast_to->kind));
		}

		break;
	case EXPRESSION_CALL:
		if (!sema_analyse_call_expression(sema_context, expression))
			return false;

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

	case STATEMENT_IF:
		if (!sema_analyse_expression(sema_context, statement->if_.condition))
			return false;

		if (statement->if_.condition->type->kind != TYPE_KIND_BOOL)
		{
			sema_report_error(
			    &statement->if_.condition->source_span,
			    "Condition of 'if' statement must be of type '" YHRT("bool") "', but it is of type '" YHRT("%s") "'.",
			    type_kind_to_string(statement->if_.condition->type->kind));

			return false;
		}

		if (!sema_analyse_statement(sema_context, statement->if_.then_branch))
			return false;

		if (statement->if_.else_branch != nullptr)
			if (!sema_analyse_statement(sema_context, statement->if_.else_branch))
				return false;

		return true;

	case STATEMENT_WHILE:
		if (!sema_analyse_expression(sema_context, statement->while_.condition))
			return false;

		if (statement->while_.condition->type->kind != TYPE_KIND_BOOL)
		{
			sema_report_error(&statement->while_.condition->source_span,
			                  "Condition of 'while' statement must be of type '" YHRT(
			                      "bool") "', but it is of type '" YHRT("%s") "'.",
			                  type_kind_to_string(statement->while_.condition->type->kind));
			return false;
		}

		if (!sema_analyse_statement(sema_context, statement->while_.body))
			return false;

		return true;

	default:
		break;
	}

	UNREACHABLE;
}

bool sema_analyse_function_declaration_shallow(SemaContext* sema_context, Declaration* declaration)
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

	hash_map_set(&sema_context->defined_functions, declaration->function.signature.name, declaration);

	return true;
}

bool sema_analyse_function_declaration_deep(SemaContext* sema_context, Declaration* declaration)
{
	bool result = false;

	sema_context->current_function = declaration;

	bool is_foreign = has_attribute(declaration->function.signature.attributes, ATTRIBUTE_FOREIGN);

	if (declaration->function.signature.parameters != nullptr)
	{
		if (vector_get_length(declaration->function.signature.parameters) > MAX_FN_PARAMETERS)
		{
			sema_report_error(&declaration->function.signature.source_span,
			                  "Function '%s' has too many parameters (%d). Maximum is %d.",
			                  declaration->function.signature.name,
			                  vector_get_length(declaration->function.signature.parameters), MAX_FN_PARAMETERS);
			return false;
		}

		for (uint64 i = 0; i < vector_get_length(declaration->function.signature.parameters); ++i)
		{
			Declaration* parameter    = declaration->function.signature.parameters[i];
			parameter->resolve_status = RESOLVE_STATUS_RESOLVING;

			if (!is_foreign)
			{
				if (parameter->variable.name == nullptr)
				{
					sema_report_error(&parameter->source_span,
					                  "Parameter of function '%s' does not have a name. Please provide a name.",
					                  declaration->function.signature.name);
					return false;
				}
			}

			if (parameter->variable.initializer != nullptr)
			{
				if (!sema_analyse_expression(sema_context, parameter->variable.initializer))
					return false;

				Type* parameter_type   = parameter->variable.type;
				Type* initializer_type = parameter->variable.initializer->type;

				if (parameter_type != initializer_type)
				{
					sema_report_error(&parameter->source_span,
					                  "Parameter '%s' of type '%s' cannot be initialized with expression of type '%s'.",
					                  parameter->variable.name, type_kind_to_string(parameter_type->kind),
					                  type_kind_to_string(initializer_type->kind));

					return false;
				}
			}

			parameter->resolve_status = RESOLVE_STATUS_RESOLVED;
		}
	}

	if (is_foreign)
	{
		if (declaration->function.body)
		{
			sema_report_error(
			    &declaration->function.body->source_span,
			    "Function '%s' is declared as foreign, but it has a body! Foreign functions cannot have a body.",
			    declaration->function.signature.name);

			return false;
		}
	}
	else
	{
		if (declaration->function.body == nullptr)
		{
			sema_report_error(
			    &declaration->function.signature.source_span,
			    "Function '%s' does not have a body! Please provide a body for the function. If you want to declare a "
			    "foreign function, please use the 'foreign' attribute.",
			    declaration->function.signature.name);
		}

		sema_push_scope(sema_context, declaration->function.body);

		result = sema_analyse_compound_statement(sema_context, declaration->function.body);

		sema_pop_scope(sema_context);

		// TODO: verify that function return type matches those with the return statements, or not require return if
		// void

		// Statement* function_body = declaration->function.body;
	}

	declaration->resolve_status = RESOLVE_STATUS_RESOLVED;

	hash_map_set(&sema_context->analysed_functions, declaration->function.signature.name, declaration);

	return result;
}

void sema_analyse_parsed_context(Context* context)
{
	TRACE(ANSI_COLOR_CYAN "Semantic analysis file...\n" ANSI_COLOR_RESET);

	uint64 function_count = vector_get_length(context->functions_declarations);

	SemaContext sema_context        = {0};
	sema_context.defined_functions  = hash_map_create(sizeof(Declaration*), function_count + 1);
	sema_context.analysed_functions = hash_map_create(sizeof(Declaration*), function_count + 1);

	Clock clock = clock_create();

	// Sema I pass - check for function declarations
	for (uint64 i = 0; i < function_count; ++i)
		sema_analyse_function_declaration_shallow(&sema_context, context->functions_declarations[i]);

	// Sema II pass - analyse functions
	for (uint64 i = 0; i < function_count; ++i)
		sema_analyse_function_declaration_deep(&sema_context, context->functions_declarations[i]);

	// check if main function is defined
	// do not check it if there are any errors, because it could be a false positive
	if (context->error_count == 0)
	{
		Declaration* main_function = sema_try_get_analysed_function(&sema_context, "main");

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

	hash_map_destroy(&sema_context.defined_functions);
	hash_map_destroy(&sema_context.analysed_functions);

	TRACE(ANSI_COLOR_CYAN "Semantic analysis took %f ms\n" ANSI_COLOR_RESET, clock_get_elapsed_time(&clock) * 1000.0f);
}
