#include "compiler_internal.h"

void compiler_internal_initialize(void)
{
	arena_allocator_create(&string_allocator, "STRING allocator", MB(1));

	symbol_table = hash_map_create(sizeof(TokenType), TOKEN_KEYWORD_COUNT);

	hash_map_set(&symbol_table, token_type_to_string(TOKEN_FUNCTION_KEYWORD), TOKEN_FUNCTION_KEYWORD);
	hash_map_set(&symbol_table, token_type_to_string(TOKEN_RETURN_KEYWORD), TOKEN_RETURN_KEYWORD);
	hash_map_set(&symbol_table, token_type_to_string(TOKEN_INT32_KEYWORD), TOKEN_INT32_KEYWORD);
}

void compiler_internal_shutdown(void)
{
	arena_allocator_print_stats(&string_allocator);
	arena_allocator_destroy(&string_allocator);
}
