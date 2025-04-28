#include "compiler_internal.h"

void compiler_internal_initialize(void)
{
	arena_allocator_create(&string_allocator, "STRING allocator", MB(1));
	arena_allocator_create(&declaration_allocator, "DECLARATION allocator", MB(1));

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

	hash_map_destroy(&symbol_table);
	hash_map_destroy(&type_table);

	vector_destroy(global_context.functions_declarations);
}

void global_context_emit_functions_json(Context* context, FILE* file)
{
	fputs("\t\"functions\": [\n", file);

	for (uint64 i = 0; i < vector_get_length(global_context.functions_declarations); ++i)
	{
		Declaration* declaration     = global_context.functions_declarations[i];
		FunctionDeclaration function = declaration->function;

		fputs("\t\t{\n", file);
		fputs("\t\t\t\"name\": \"", file);
		fputs(function.signature.name, file);
		fputs("\",\n", file);
		fputs("\t\t\t\"parameters\": [\n", file);

		fputs("\t\t\t]\n", file);
		fputs("\t\t},\n", file);
	}

	fputs("\t]", file);
}

void global_context_emit_json_to_file(Context* context, FILE* file)
{
	TRACE("Emitting parsed global context as JSON to %s...\n", file == stdout ? "console" : "file");

	fputs("{\n", file);
	global_context_emit_functions_json(context, file);
	fputs("\n}\n", file);

	TRACE("JSON emitted successfully!\n");
}

void global_context_emit_json(Context* context)
{
	global_context_emit_json_to_file(context, stdout);
}
