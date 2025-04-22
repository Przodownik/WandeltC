#include "memory.h"

void arena_allocator_create(ArenaAllocator* allocator, const char* name, uint64 size)
{
	ASSERT(allocator != nullptr, "Allocator is nullptr");
	ASSERT(name != nullptr, "Allocator name is nullptr");
	ASSERT(size > 0, "Cannot create allocator %s with size %llu", name, size);
	ASSERT(size % 16 == 0, "Allocator size must be a multiple of 16 bytes");

	allocator->name        = name;
	allocator->memory      = calloc(size, sizeof(int8));
	allocator->size        = size;
	allocator->used        = 0u;
	allocator->allocations = 0u;
}

void arena_allocator_destroy(ArenaAllocator* allocator)
{
	free(allocator->memory);
}

void* arena_allocator_allocate(ArenaAllocator* allocator, uint64 size)
{
	VERIFY(size > 0, "Cannot allocate %llu bytes", size);

	size = (size + 15) & ~15; // round up to 16 bytes

	VERIFY(allocator->used + size <= allocator->size, "Allocator %s is out of memory", allocator->name);

	void* ptr = (uint8*)allocator->memory + allocator->used;

	allocator->used += size;
	allocator->allocations++;

	return ptr;
}

void arena_allocator_print_stats(ArenaAllocator* allocator)
{
	TRACE(ANSI_COLOR_YELLOW "Allocator: %s\n" ANSI_COLOR_RESET, allocator->name);
	TRACE(ANSI_COLOR_YELLOW " Memory used / total (bytes): %llu b / %llu b\n" ANSI_COLOR_RESET, allocator->used,
	      allocator->size);
	TRACE(ANSI_COLOR_YELLOW " Memory used / total (kb): %llu kb / %llu kb\n" ANSI_COLOR_RESET, allocator->used / KB(1),
	      allocator->size / KB(1));
	TRACE(ANSI_COLOR_YELLOW " Allocations: %llu\n" ANSI_COLOR_RESET, allocator->allocations);
}
