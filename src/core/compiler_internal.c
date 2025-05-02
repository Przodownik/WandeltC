#include "compiler_internal.h"

#include "vendor/cJSON/cJSON.h"

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

const char* binary_operator_to_string(BinaryOperator op)
{
	switch (op)
	{
	case BINARY_OPERATOR_ADD:
		return "+";
	case BINARY_OPERATOR_SUBTRACT:
		return "-";
	case BINARY_OPERATOR_MULTIPLY:
		return "*";
	case BINARY_OPERATOR_DIVIDE:
		return "/";
	default:
		return "unknown";
	}
}

void emit_expression_json(Expression* expr, cJSON* parent)
{
	if (!expr)
	{
		cJSON_AddNullToObject(parent, "expression");
		return;
	}

	cJSON* obj = cJSON_CreateObject();
	switch (expr->kind)
	{
	case EXPRESSION_LITERAL:
		cJSON_AddStringToObject(obj, "kind", "literal");
		cJSON_AddNumberToObject(obj, "value", expr->literal.int_value);
		break;

	case EXPRESSION_GROUP:
		cJSON_AddStringToObject(obj, "kind", "group");
		if (expr->group.expression)
		{
			cJSON* group = cJSON_CreateObject();
			emit_expression_json(expr->group.expression, group);
			cJSON_AddItemToObject(obj, "value", group);
		}
		break;
	case EXPRESSION_BINARY:
		cJSON_AddStringToObject(obj, "kind", "binary");
		cJSON_AddStringToObject(obj, "operator", binary_operator_to_string(expr->binary.operator));
		cJSON* left  = cJSON_CreateObject();
		cJSON* right = cJSON_CreateObject();
		emit_expression_json(expr->binary.left, left);
		emit_expression_json(expr->binary.right, right);
		cJSON_AddItemToObject(obj, "left", left);
		cJSON_AddItemToObject(obj, "right", right);
		break;

	default:
		cJSON_AddStringToObject(obj, "kind", "unknown");
		cJSON_AddNullToObject(obj, "value");
		break;
	}

	cJSON_AddItemToObject(parent, "expression", obj);
}

void emit_statement_json(Statement* stmt, cJSON* array)
{
	if (!stmt)
	{
		cJSON_AddItemToArray(array, cJSON_CreateNull());
		return;
	}

	cJSON* obj = cJSON_CreateObject();

	switch (stmt->type)
	{
	case STATEMENT_RETURN:
		cJSON_AddStringToObject(obj, "type", "return");
		emit_expression_json(stmt->return_.expression, obj);
		break;

	case STATEMENT_COMPOUND: {
		cJSON_AddStringToObject(obj, "type", "compound");
		cJSON* body_array = cJSON_CreateArray();

		Statement* inner = stmt->compound.first;
		while (inner)
		{
			emit_statement_json(inner, body_array);
			inner = inner->next;
		}

		cJSON_AddItemToObject(obj, "body", body_array);
		break;
	}

	default:
		cJSON_AddStringToObject(obj, "type", "unknown");
		break;
	}

	cJSON_AddItemToArray(array, obj);
}

void emit_declaration_json(Declaration* decl, cJSON* element)
{
	ASSERT(decl != NULL, "Declaration is null!");

	cJSON* json_decl = cJSON_CreateObject();
	cJSON_AddStringToObject(json_decl, "kind", decl->kind == DECLARATION_FUNCTION ? "function" : "unknown");

	cJSON* data = cJSON_CreateObject();
	cJSON_AddStringToObject(data, "name", decl->function.signature.name);

	cJSON* body_array = cJSON_CreateArray();
	emit_statement_json(decl->function.body, body_array);
	cJSON_AddItemToObject(data, "body", body_array);

	cJSON_AddItemToObject(json_decl, "data", data);
	cJSON_AddItemToArray(element, json_decl);
}

void global_context_emit_functions_json(Context* context, cJSON* element)
{
	uint64 len = vector_get_length(context->functions_declarations);
	for (uint64 i = 0; i < len; ++i)
	{
		Declaration* declaration = context->functions_declarations[i];
		emit_declaration_json(declaration, element);
	}
}

void global_context_emit_json_to_file(Context* context, FILE* file)
{
	TRACE("Emitting parsed global context as JSON to %s...\n", file == stdout ? "console" : "file");

	cJSON* root = cJSON_CreateArray();
	global_context_emit_functions_json(context, root);

	FILE* json_file = fopen("output.json", "w+");
	if (json_file == nullptr)
	{
		ERROR("Could not open file output.json for writing.\n");
		cJSON_Delete(root);
		return;
	}
	char* printed_json = cJSON_Print(root);
	fwrite(printed_json, sizeof(char), strlen(printed_json), json_file);
	fclose(json_file);

	// fprintf(file, "%s\n", printed_json);

	free(printed_json);
	cJSON_Delete(root);
}

void global_context_emit_json(Context* context)
{
	global_context_emit_json_to_file(context, stdout);
}
