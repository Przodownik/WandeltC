#include "core/compiler.h"

extern ArenaAllocator string_allocator;

int32 main(void)
{
	FILE* config = fopen(".wdt.conf", "r");
	ASSERT(config != nullptr, "Could not find or open the .wdt.conf file. It is necessary to compile.");

	arena_allocator_create(&string_allocator, "STRING allocator", MB(1));

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
			if (!filesystem_file_exists(line))
			{
				ERROR("File: %s not found but mentioned in the [ProjectFiles], skipping... \n", line);
				continue;
			}

			char* str = arena_allocator_allocate(&string_allocator, sizeof(char) * (length + 1));
			strcpy_s(str, length + 1, line);
			vector_push(file_sources, str);
		}
	}

	fclose(config);

	Compiler compiler;
	compiler_create(&compiler);

	compiler.build_options.file_sources = file_sources;



	/*for (uint64 i = 0; i < vector_get_length(compiler.build_options.file_sources); i++)
	{
		TRACE("%s\n", compiler.build_options.file_sources[i]);
	}*/

	arena_allocator_print_stats(&string_allocator);
	arena_allocator_destroy(&string_allocator);

	vector_destroy(file_sources);

	return 0;
}
