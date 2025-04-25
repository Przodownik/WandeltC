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
