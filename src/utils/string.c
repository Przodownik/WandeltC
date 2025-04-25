#include "string.h"

File file_create(const char* path)
{
	File result = {0};
	result.path = path;

	FILE* file = fopen(path, "rb");
	ASSERT(file, "Failed to open %s file!", path);

	fseek(file, 0, SEEK_END);

	const uint64 file_size = ftell(file);
	result.content_size    = file_size;
	rewind(file);

	char* buffer = calloc(file_size + 1, sizeof(char));
	ASSERT(buffer, "Failed to allocate memory for file content!");

	const uint64 bytes_read = fread(buffer, sizeof(char), file_size, file);
	ASSERT(bytes_read == file_size, "Failed to read file!");

	buffer[file_size] = '\0';
	result.content    = buffer;

	fclose(file);

	return result;
}

void file_destroy(File* file)
{
	free((char*)file->content);
}
