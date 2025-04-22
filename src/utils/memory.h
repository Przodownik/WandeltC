/**
 * @file memory.h
 * @author TF
 * @copyright Copyright (c) 2023 TF
 */
#pragma once

/**
 * @struct ArenaAllocator
 * @brief Structure representing an arena allocator.
 */
typedef struct _ArenaAllocator
{
	const char* name;   // Name of the allocator
	void* memory;       // Pointer to the allocated memory
	uint64 size;        // Size of the allocated memory
	uint64 used;        // Amount of memory used
	uint64 allocations; // Number of allocations made
} ArenaAllocator;

/**
 * @brief Creates an arena allocator.
 * @param allocator Pointer to the arena allocator to create.
 * @param name Name of the allocator.
 * @param size Size of the memory to allocate for the arena.
 */
void arena_allocator_create(ArenaAllocator* allocator, const char* name, uint64 size);

/**
 * @brief Destroys the arena allocator (frees its arena memory).
 * @param allocator Pointer to the arena allocator to destroy.
 */
void arena_allocator_destroy(ArenaAllocator* allocator);

/**
 * @brief Allocates memory from the arena allocator.
 * @param allocator Pointer to the arena allocator to allocate from.
 * @param size Size of the memory to allocate.
 * @return Pointer to the allocated memory.
 */
void* arena_allocator_allocate(ArenaAllocator* allocator, uint64 size);

/**
 * @brief Frees the memory allocated by the arena allocator.
 * @param allocator Pointer to the arena allocator to free memory from.
 * @param ptr Pointer to the memory to free.
 */
void arena_allocator_print_stats(ArenaAllocator* allocator);
