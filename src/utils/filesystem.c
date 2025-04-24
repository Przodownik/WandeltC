#include "filesystem.h"

#include <sys/stat.h>

bool filesystem_file_exists(const char* path)
{
	struct stat buffer;

	return stat(path, &buffer) == 0;
}
