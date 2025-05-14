#include "compiler_internal.h"

#include "vendor/cJSON/cJSON.h"

static Type void_type   = {.kind = TYPE_KIND_VOID};
static Type bool_type   = {.kind = TYPE_KIND_BOOL};
static Type char_type   = {.kind = TYPE_KIND_CHAR};
static Type uchar_type  = {.kind = TYPE_KIND_UCHAR};
static Type short_type  = {.kind = TYPE_KIND_SHORT};
static Type ushort_type = {.kind = TYPE_KIND_USHORT};
static Type int_type    = {.kind = TYPE_KIND_INT};
static Type uint_type   = {.kind = TYPE_KIND_UINT};
static Type long_type   = {.kind = TYPE_KIND_LONG};
static Type ulong_type  = {.kind = TYPE_KIND_ULONG};
static Type float_type  = {.kind = TYPE_KIND_FLOAT};
static Type double_type = {.kind = TYPE_KIND_DOUBLE};

void compiler_internal_initialize(void)
{
	arena_allocator_create(&string_allocator, "STRING allocator", MB(1));
	arena_allocator_create(&declaration_allocator, "DECLARATION allocator", MB(1));
	arena_allocator_create(&statement_allocator, "STATEMENT allocator", MB(1));
	arena_allocator_create(&expression_allocator, "EXPRESSION allocator", MB(1));

	symbol_table = hash_map_create(sizeof(TokenType), TOKEN_KEYWORD_COUNT);

	hash_map_set(&symbol_table, token_type_to_string(TOKEN_FUNCTION_KEYWORD), TOKEN_FUNCTION_KEYWORD);
	hash_map_set(&symbol_table, token_type_to_string(TOKEN_RETURN_KEYWORD), TOKEN_RETURN_KEYWORD);
	hash_map_set(&symbol_table, token_type_to_string(TOKEN_TRUE_KEYWORD), TOKEN_TRUE_KEYWORD);
	hash_map_set(&symbol_table, token_type_to_string(TOKEN_FALSE_KEYWORD), TOKEN_FALSE_KEYWORD);
	hash_map_set(&symbol_table, token_type_to_string(TOKEN_PUBLIC_KEYWORD), TOKEN_PUBLIC_KEYWORD);
	hash_map_set(&symbol_table, token_type_to_string(TOKEN_PRIVATE_KEYWORD), TOKEN_PRIVATE_KEYWORD);
	hash_map_set(&symbol_table, token_type_to_string(TOKEN_IF_KEYWORD), TOKEN_IF_KEYWORD);
	hash_map_set(&symbol_table, token_type_to_string(TOKEN_ELSE_KEYWORD), TOKEN_ELSE_KEYWORD);

	hash_map_set(&symbol_table, token_type_to_string(TOKEN_VOID_KEYWORD), TOKEN_VOID_KEYWORD);
	hash_map_set(&symbol_table, token_type_to_string(TOKEN_BOOL_KEYWORD), TOKEN_BOOL_KEYWORD);
	hash_map_set(&symbol_table, token_type_to_string(TOKEN_CHAR_KEYWORD), TOKEN_CHAR_KEYWORD);
	hash_map_set(&symbol_table, token_type_to_string(TOKEN_UCHAR_KEYWORD), TOKEN_UCHAR_KEYWORD);
	hash_map_set(&symbol_table, token_type_to_string(TOKEN_SHORT_KEYWORD), TOKEN_SHORT_KEYWORD);
	hash_map_set(&symbol_table, token_type_to_string(TOKEN_USHORT_KEYWORD), TOKEN_USHORT_KEYWORD);
	hash_map_set(&symbol_table, token_type_to_string(TOKEN_INT_KEYWORD), TOKEN_INT_KEYWORD);
	hash_map_set(&symbol_table, token_type_to_string(TOKEN_UINT_KEYWORD), TOKEN_UINT_KEYWORD);
	hash_map_set(&symbol_table, token_type_to_string(TOKEN_LONG_KEYWORD), TOKEN_LONG_KEYWORD);
	hash_map_set(&symbol_table, token_type_to_string(TOKEN_ULONG_KEYWORD), TOKEN_ULONG_KEYWORD);
	hash_map_set(&symbol_table, token_type_to_string(TOKEN_FLOAT_KEYWORD), TOKEN_FLOAT_KEYWORD);
	hash_map_set(&symbol_table, token_type_to_string(TOKEN_DOUBLE_KEYWORD), TOKEN_DOUBLE_KEYWORD);

	type_table = hash_map_create(sizeof(Type*), TYPE_KIND_COUNT);
	hash_map_set(&type_table, token_type_to_string(TOKEN_VOID_KEYWORD), &void_type);
	hash_map_set(&type_table, token_type_to_string(TOKEN_BOOL_KEYWORD), &bool_type);
	hash_map_set(&type_table, token_type_to_string(TOKEN_CHAR_KEYWORD), &char_type);
	hash_map_set(&type_table, token_type_to_string(TOKEN_UCHAR_KEYWORD), &uchar_type);
	hash_map_set(&type_table, token_type_to_string(TOKEN_SHORT_KEYWORD), &short_type);
	hash_map_set(&type_table, token_type_to_string(TOKEN_USHORT_KEYWORD), &ushort_type);
	hash_map_set(&type_table, token_type_to_string(TOKEN_INT_KEYWORD), &int_type);
	hash_map_set(&type_table, token_type_to_string(TOKEN_UINT_KEYWORD), &uint_type);
	hash_map_set(&type_table, token_type_to_string(TOKEN_LONG_KEYWORD), &long_type);
	hash_map_set(&type_table, token_type_to_string(TOKEN_ULONG_KEYWORD), &ulong_type);
	hash_map_set(&type_table, token_type_to_string(TOKEN_FLOAT_KEYWORD), &float_type);
	hash_map_set(&type_table, token_type_to_string(TOKEN_DOUBLE_KEYWORD), &double_type);

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

const char* type_kind_to_string(TypeKind kind)
{
	switch (kind)
	{
	case TYPE_KIND_FUNCTION:
		return "function";

	case TYPE_KIND_VOID:
		return "void";
	case TYPE_KIND_BOOL:
		return "bool";
	case TYPE_KIND_CHAR:
		return "char";
	case TYPE_KIND_UCHAR:
		return "uchar";
	case TYPE_KIND_SHORT:
		return "short";
	case TYPE_KIND_USHORT:
		return "ushort";
	case TYPE_KIND_INT:
		return "int";
	case TYPE_KIND_UINT:
		return "uint";
	case TYPE_KIND_LONG:
		return "long";
	case TYPE_KIND_ULONG:
		return "ulong";
	case TYPE_KIND_FLOAT:
		return "float";
	case TYPE_KIND_DOUBLE:
		return "double";

	default:
		return "unknown";
	}
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
	case BINARY_OPERATOR_EQUAL:
		return "==";
	case BINARY_OPERATOR_NOT_EQUAL:
		return "!=";
	case BINARY_OPERATOR_GREATER:
		return ">";
	case BINARY_OPERATOR_LESS:
		return "<";
	case BINARY_OPERATOR_GREATER_OR_EQUAL:
		return ">=";
	case BINARY_OPERATOR_LESS_OR_EQUAL:
		return "<=";
	case BINARY_OPERATOR_ASSIGN:
		return "=";
	default:
		return "unknown";
	}
}

const char* unary_operator_to_string(UnaryOperator op)
{
	switch (op)
	{
	case UNARY_OPERATOR_NEGATE:
		return "-";
	default:
		return "unknown";
	}
}

const char* assign_operator_to_string(AssignOperator op)
{
	switch (op)
	{
	case ASSIGN_OPERATOR_ASSIGN:
		return "=";
	default:
		return "unknown";
	}
}

void emit_declaration_json(Declaration* decl, cJSON* element);

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
	case EXPRESSION_CONSTANT:
		cJSON_AddStringToObject(obj, "kind", "literal");
		cJSON_AddNumberToObject(obj, "value", (double)expr->constant.int_value);
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
	case EXPRESSION_IDENTIFIER:
		cJSON_AddStringToObject(obj, "kind", "identifier");
		cJSON_AddStringToObject(obj, "name", expr->identifier.name);
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

	switch (stmt->kind)
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

	case STATEMENT_DECLARATION: {
		emit_declaration_json(stmt->declaration.declaration, obj);
		break;
	}

	case STATEMENT_EXPRESSION:
		emit_expression_json(stmt->expression.expression, obj);
		break;

	case STATEMENT_IF: {
		cJSON_AddStringToObject(obj, "type", "if");
		cJSON* condition = cJSON_CreateObject();
		emit_expression_json(stmt->if_.condition, condition);
		cJSON_AddItemToObject(obj, "condition", condition);
		cJSON* then_branch = cJSON_CreateObject();
		emit_statement_json(stmt->if_.then_branch, then_branch);
		cJSON_AddItemToObject(obj, "then_branch", then_branch);
		if (stmt->if_.else_branch)
		{
			cJSON* else_branch = cJSON_CreateObject();
			emit_statement_json(stmt->if_.else_branch, else_branch);
			cJSON_AddItemToObject(obj, "else_branch", else_branch);
		}
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

	switch (decl->kind)
	{
	case DECLARATION_FUNCTION:
		cJSON* json_decl = cJSON_CreateObject();
		cJSON_AddStringToObject(json_decl, "kind", "function");
		cJSON_AddStringToObject(json_decl, "name", decl->function.signature.name);
		cJSON* body_array = cJSON_CreateArray();
		emit_statement_json(decl->function.body, body_array);
		cJSON_AddItemToObject(json_decl, "body", body_array);
		cJSON_AddItemToArray(element, json_decl);
		break;

	case DECLARATION_VARIABLE:
		cJSON_AddStringToObject(element, "kind", "variable");
		cJSON_AddStringToObject(element, "name", decl->variable.name);
		cJSON* initializer = cJSON_CreateObject();
		emit_expression_json(decl->variable.initializer, initializer);
		cJSON_AddItemToObject(element, "initializer", initializer);
		break;

	default:
		cJSON_AddStringToObject(element, "kind", "unknown");
		cJSON_AddNullToObject(element, "value");
	}
}

void global_context_emit_functions_json(Context* context, void* element)
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

uint32 get_index_from_position(const File* file, uint32 row, uint32 column)
{
	uint32 index       = 0;
	uint32 current_row = 1;

	const char* p = file->content;

	while (current_row < row && p < file->content + file->content_size)
	{
		if (*p == '\n')
			current_row++;

		p++;
		index++;
	}

	index += column - 1; // Adjust to 1-based column

	return index;
}

void get_position_from_index(const File* file, uint32 index, uint32* row, uint32* column)
{
	uint32 r = 1, c = 1;
	const char* p = file->content;

	for (uint32 i = 0; i < index && p < file->content + file->content_size; i++, p++)
	{
		if (*p == '\n')
		{
			r++;
			c = 1;
		}
		else
		{
			c++;
		}
	}

	*row    = r;
	*column = c;
}

uint32 get_display_column(const char* line_start, uint32 char_column)
{
	uint32 display_col = 1;
	const char* p      = line_start;

	for (uint32 i = 1; i < char_column && p < line_start + strlen(line_start); i++)
	{
		if (*p == '\t')
		{
			display_col += TAB_SIZE; // Tab occupies tab_width columns
		}
		else
		{
			display_col += 1; // Non-tab character occupies 1 column
		}
		p++;
	}
	return display_col;
}

SourceSpan extend_span_with_token(SourceSpan loc, SourceSpan after)
{
	ASSERT(loc.source_file == after.source_file, "Source files must match");

	uint32 start_index  = get_index_from_position(loc.source_file, loc.row, loc.column);
	uint32 end_index    = get_index_from_position(after.source_file, after.row, after.column) + after.length;
	uint32 total_length = end_index - start_index;

	SourceSpan extended = loc;
	extended.length     = total_length;

	return extended;
}
