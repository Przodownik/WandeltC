/**
 * @file string.h
 * @author TF
 * @copyright Copyright (c) 2025 TF
 */
#pragma once

/**
 * @brief File structure to hold file information.
 */
typedef struct _File
{
	const char* path;    // Path to the file (relative to the project root)
	const char* content; // Content of the file
	uint64 content_size; // Size of the content in bytes
} File;

/**
 * @brief Read the content of a file into a file structure.
 *
 * @param path Path to the file (relative to the project root)
 * @return A file structure containing the file information.
 */
File file_create(const char* path);

/**
 * @brief Destroy the file structure and free its memory.
 *
 * @param file A pointer to the file structure to be destroyed.
 */
void file_destroy(File* file);

/**
 * @brief A structure representing a view of a C-style string.
 */
typedef struct _CStringView
{
	const char* start; // Pointer to the start of the string
	uint32 length;     // Length of the string in bytes
} CStringView;

/**
 * @brief Compare two C-style string views for equality.
 *
 * @param str1 Pointer to the first string view.
 * @param str2 Pointer to the second string view.
 * @return true if the strings are equal, false otherwise.
 */
bool cstring_view_equals(const CStringView* str1, const CStringView* str2);

/**
 * @brief Compare a C-style string view with a C-style string for equality.
 *
 * @param str1 Pointer to the string view.
 * @param str2 Pointer to the C-style string.
 * @return true if the strings are equal, false otherwise.
 */
bool cstring_view_equals_cstr(const CStringView* str1, const char* str2);

/**
 * @brief Copy a C-style string into a buffer.
 *
 * @param str Pointer to the source string.
 * @param length Length of the string to copy.
 * @param buffer Pointer to the destination buffer. Must be length + 1 bytes long for null-termination.
 * @return Pointer to the copied string in the buffer.
 */
char* cstring_copy_part_into_buffer(const char* str, uint32 length, void* buffer);

/**
 * @brief Format a string using a format string and a variable argument list.
 * @warning The returned string must be freed after use.
 *
 * @param format Format string.
 * @param args Variable argument list.
 * @return Pointer to the formatted string. Must be freed after use.
 */
char* cstring_vformat(const char* format, va_list args);
