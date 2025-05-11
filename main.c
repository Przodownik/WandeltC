#include "core/compiler.h"

extern jmp_buf on_error_jump;           // from defines.h
extern ArenaAllocator string_allocator; // from compiler_internal.h

int32 main(int32 argc, char** argv)
{
	int result = setjmp(on_error_jump);
	if (result)
	{
		DEBUG_BREAK();
		return result;
	}

	compiler_internal_initialize();

	File* file_sources = vector_create(5, sizeof(File));

	if (argc > 1)
	{
		// --files filename1 filename2 filename3
		for (int32 i = 1; i < argc; ++i)
		{
			if (strcmp(argv[i], "--files") == 0)
			{
				for (int32 j = i + 1; j < argc; ++j)
				{
					if (argv[j][0] == '-')
						break;

					if (!filesystem_file_exists(argv[j]))
					{
						ERROR("File: %s not found but mentioned in the --files, skipping... \n", argv[j]);
						continue;
					}

					File source_file = file_create(argv[j]);
					vector_push(file_sources, source_file);
				}
				break;
			}
		}

		if (vector_get_length(file_sources) == 0)
		{
			ERROR("No files provided in the --files argument.\n");
			return -1;
		}
	}
	else
	{
		FILE* config = fopen(".wdt.conf", "r");
		// FILE* config = fopen("../../../example/.wdt.conf", "r");

		ASSERT(config != nullptr, "Could not find or open the .wdt.conf file. It is necessary to compile.");

		char line[256];
		bool in_project_files = false;

		while (fgets(line, sizeof(line), config))
		{
			// Strip newline characters
			line[strcspn(line, "\r\n")] = 0;

			// skip everything after #
			char* comment = strchr(line, '#');
			if (comment != nullptr)
				*comment = '\0';

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

				File source_file = file_create(line);
				vector_push(file_sources, source_file);
			}
		}

		fclose(config);
	}

	Compiler compiler;
	compiler_create(&compiler);

	compiler.build_options.file_sources = file_sources;
	compiler.build_options.lexer_debug  = true;
	compiler.build_options.parsed_debug = true;

	compiler_compile(&compiler);

	vector_destroy(file_sources);

	compiler_internal_shutdown();

	return global_context.error_count + global_context.warning_count;
}
