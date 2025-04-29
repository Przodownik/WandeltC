#include "compiler_internal.h"

void compiler_internal_initialize(void)
{
	arena_allocator_create(&string_allocator, "STRING allocator", MB(1));
	arena_allocator_create(&declaration_allocator, "DECLARATION allocator", MB(1));
	arena_allocator_create(&statement_allocator, "STATEMENT allocator", MB(1));
	arena_allocator_create(&expression_allocator, "EXPRESSION allocator", MB(1));

	symbol_table = hash_map_create(sizeof(TokenType), TOKEN_KEYWORD_COUNT);
	hash_map_set(&symbol_table, token_type_to_string(TOKEN_FUNCTION_KEYWORD), TOKEN_FUNCTION_KEYWORD);
	hash_map_set(&symbol_table, token_type_to_string(TOKEN_RETURN_KEYWORD), TOKEN_RETURN_KEYWORD);
	hash_map_set(&symbol_table, token_type_to_string(TOKEN_INT32_KEYWORD), TOKEN_INT32_KEYWORD);
	hash_map_set(&symbol_table, token_type_to_string(TOKEN_PUBLIC_KEYWORD), TOKEN_PUBLIC_KEYWORD);
	hash_map_set(&symbol_table, token_type_to_string(TOKEN_PRIVATE_KEYWORD), TOKEN_PRIVATE_KEYWORD);

	type_table = hash_map_create(sizeof(TypeKind), TYPE_KIND_COUNT);
	hash_map_set(&type_table, "int32", TYPE_KIND_INT_32);

	global_context.functions_declarations = vector_create(10, sizeof(Declaration*));
}

void compiler_internal_shutdown(void)
{
	arena_allocator_print_stats(&string_allocator);
	arena_allocator_destroy(&string_allocator);

	arena_allocator_print_stats(&declaration_allocator);
	arena_allocator_destroy(&declaration_allocator);

	arena_allocator_print_stats(&statement_allocator);
	arena_allocator_destroy(&statement_allocator);

	arena_allocator_print_stats(&expression_allocator);
	arena_allocator_destroy(&expression_allocator);

	hash_map_destroy(&symbol_table);
	hash_map_destroy(&type_table);

	vector_destroy(global_context.functions_declarations);
}

void emit_expression_json(Expression* expr, FILE* file, int indent)
{
	if (!expr)
	{
		fprintf(file, "%*snull", indent * 4, "");
		return;
	}

	fprintf(file, "%*s{\n", 0, "");
	fprintf(file, "%*s\"kind\": \"", (indent + 1) * 4, "");
	switch (expr->kind)
	{
	case EXPRESSION_LITERAL:
		fputs("literal expression", file);
		break;
	default:
		fputs("UNKNOWN", file);
		break;
	}
	fprintf(file, "\",\n");
	fprintf(file, "%*s\"value\": ", (indent + 1) * 4, "");
	switch (expr->kind)
	{
	case EXPRESSION_LITERAL:
		fprintf(file, "\"%i\"", expr->literal.int_value);
		break;
	default:
		fputs("null", file);
		break;
	}
	fprintf(file, "\n%*s}", indent * 4, "");
}

void emit_statement_json(Statement* stmt, FILE* file, int indent)
{
	if (!stmt)
	{
		fprintf(file, "%*snull", indent * 4, "");
		return;
	}

	fprintf(file, "%*s{\n", 0, "");
	fprintf(file, "%*s\"type\": \"", (indent + 1) * 4, "");
	switch (stmt->type)
	{
	case STATEMENT_RETURN:
		fputs("return statement", file);
		break;
	case STATEMENT_COMPOUND:
		fputs("compound statement", file);
		break;
	case STATEMENT_INVALID:
		fputs("INVALID", file);
		break;
	default:
		fputs("UNKNOWN", file);
		break;
	}
	fprintf(file, "\",\n");
	fprintf(file, "%*s\"data\": ", (indent + 1) * 4, "");
	switch (stmt->type)
	{
	case STATEMENT_RETURN:
		emit_expression_json(stmt->return_.expression, file, indent + 1);
		break;
	case STATEMENT_COMPOUND: {
		fprintf(file, "[\n");
		Statement* inner = stmt->compound.first;
		while (inner)
		{
			fprintf(file, "%*s", (indent + 2) * 4, "");
			emit_statement_json(inner, file, indent + 2);
			inner = inner->next;
			if (inner)
			{
				fprintf(file, ",");
			}
			fprintf(file, "\n");
		}
		fprintf(file, "%*s]", (indent + 1) * 4, "");
		break;
	}
	default:
		fputs("null", file);
		break;
	}
	fprintf(file, "\n%*s}", indent * 4, "");
}

void emit_declaration_json(Declaration* decl, FILE* file, int indent)
{
	if (!decl)
	{
		fprintf(file, "%*snull", indent * 4, "");
		return;
	}

	fprintf(file, "%*s{\n", indent * 4, "");
	fprintf(file, "%*s\"kind\": \"", (indent + 1) * 4, "");
	switch (decl->kind)
	{
	case DECLARATION_FUNCTION:
		fputs("function declaration", file);
		break;
	default:
		fputs("UNKNOWN", file);
		break;
	}
	fprintf(file, "\",\n");
	fprintf(file, "%*s\"data\": {\n", (indent + 1) * 4, "");
	fprintf(file, "%*s\"name\": \"%s\",\n", (indent + 2) * 4, "", decl->function.signature.name);
	fprintf(file, "%*s\"body\": ", (indent + 2) * 4, "");
	emit_statement_json(decl->function.body, file, indent + 2);
	fprintf(file, "\n%*s}\n", (indent + 1) * 4, "");
	fprintf(file, "%*s}", indent * 4, "");
}

void global_context_emit_functions_json(Context* context, FILE* file)
{
	uint64 len = vector_get_length(context->functions_declarations);
	for (uint64 i = 0; i < len; ++i)
	{
		Declaration* declaration = context->functions_declarations[i];
		emit_declaration_json(declaration, file, 1);
		if (i < len - 1)
		{
			fprintf(file, ",\n");
		}
	}
}

void global_context_emit_json_to_file(Context* context, FILE* file)
{
	TRACE("Emitting parsed global context as JSON to %s...\n", file == stdout ? "console" : "file");
	fputs("[\n", file);
	global_context_emit_functions_json(context, file);
	fputs("\n]\n", file);
	TRACE("JSON emitted successfully!\n");
}

void global_context_emit_json(Context* context)
{
	global_context_emit_json_to_file(context, stdout);
}
