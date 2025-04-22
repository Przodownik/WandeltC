int main(void)
{
	// FILE* config = fopen(".wdt.conf", "r");
	// ASSERT(config != nullptr, "Could not find or open the .wdt.conf file. It is necessary to compile.");
	//  fclose(config);

	// printf("Hello %s", "world!");

	ArenaAllocator allocator;
	arena_allocator_create(&allocator, "Default allocator", MB(1));
	arena_allocator_print_stats(&allocator);
	arena_allocator_destroy(&allocator);

	int* intArray = vector_create(10, sizeof(int));

	int x    = 12;
	intArray = vector_push(intArray, &x);
	intArray = vector_push(intArray, &x);

	TRACE("%llu", vector_length(intArray));

	vector_destroy(intArray);
	//__typeof(intArray) x = 12;
	// printf("%ld\n", __STDC_VERSION__);

	// typeof(intArray) x = 12;

	return 0;
}
