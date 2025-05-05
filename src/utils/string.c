#include "string.h"

File file_create(const char* path)
{
	char* path_cpy = calloc(strlen(path) + 1, sizeof(char));
	ASSERT(path_cpy, "Failed to allocate memory for file path!");

	strcpy_s(path_cpy, strlen(path) + 1, path);

	File result = {0};
	result.path = path_cpy;

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

	// Count the number of lines in the file
	const char* p = buffer;
	while (*p)
	{
		if (*p == '\n')
			result.total_rows++;
		p++;
	}

	return result;
}

void file_destroy(File* file)
{
	free((char*)file->content);
}

bool cstring_view_equals(const CStringView* str1, const CStringView* str2)
{
	ASSERT(str1 && str2, "CStringView pointers cannot be null!");
	ASSERT(str1->start && str2->start, "CStringView start pointers cannot be null!");

	if (str1->length != str2->length)
		return false;

	for (uint32 i = 0; i < str1->length; ++i)
	{
		if (str1->start[i] != str2->start[i])
			return false;
	}

	return true;
}

bool cstring_view_equals_cstr(const CStringView* str1, const char* str2)
{
	ASSERT(str1 && str2, "CStringView and C-style string pointers cannot be null!");
	ASSERT(str1->start, "CStringView start pointer cannot be null!");

	const uint32 str2_length = (uint32)strlen(str2);

	if (str1->length != str2_length)
		return false;

	for (uint32 i = 0; i < str1->length; ++i)
	{
		if (str1->start[i] != str2[i])
			return false;
	}

	return true;
}

char* cstring_copy_part_into_buffer(const char* str, uint32 length, void* buffer)
{
	ASSERT(str, "String pointer cannot be null!");
	ASSERT(buffer, "Buffer pointer cannot be null!");

	char* result = (char*)buffer;
	memcpy(result, str, length);
	result[length] = '\0';

	return result;
}

char* cstring_vformat(const char* format, va_list args)
{
	va_list args_copy;
	va_copy(args_copy, args);

	const uint32 length = vsnprintf(NULL, 0, format, args_copy);
	va_end(args_copy);

	char* buffer = calloc(length + 1, sizeof(char));
	ASSERT(buffer, "Failed to allocate memory for formatted string!");
	vsnprintf(buffer, length + 1, format, args);

	return buffer;
}
