#include "utils/defines.h"
int32 main(void)
{
	FILE* config = fopen("../../../example/.wdt.conf", "r");
	ASSERT(config != nullptr, "Could not find or open the .wdt.conf file. It is necessary to compile.");

	ArenaAllocator allocator;
	arena_allocator_create(&allocator, "Default allocator", MB(1));

	char** file_sources = vector_create(5, sizeof(char*));

	char line[256];
	bool in_project_files = false;

	while (fgets(line, sizeof(line), config))
	{
		// Strip newline characters
		line[strcspn(line, "\r\n")] = 0;

		size_t length = strlen(line);
		if (length == 0)
			continue;

		if (line[0] == '[' && line[length - 1] == ']')
		{
			in_project_files = (strcmp(line, "[ProjectFiles]") == 0);
			continue;
		}

		// If we're inside the [ProjectFiles] section, collect lines
		if (in_project_files)
		{
			char* str = arena_allocator_allocate(&allocator, sizeof(char) * (length + 1));
			strcpy_s(str, length + 1, line);
			vector_push(file_sources, str);
		}
	}

	fclose(config);

	for (uint64 i = 0; i < vector_get_length(file_sources); i++)
	{
		printf("%s\n", file_sources[i]);
	}

	// printf("Hello %s", "world!");

	arena_allocator_print_stats(&allocator);
	arena_allocator_destroy(&allocator);

	// vector_destroy(intArray);
	//__typeof(intArray) x = 12;
	//  printf("%ld\n", __STDC_VERSION__);

	// typeof(intArray) x = 12;

	return 0;
}
