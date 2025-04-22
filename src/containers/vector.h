/**
 * @file vector.h
 * @author TF
 * @copyright Copyright (c) 2023 TF
 */
#pragma once

typedef struct _VectorHeader
{
	uint64 length;   // Number of elements in the vector
	uint64 capacity; // Capacity of the vector (number of elements it can hold)
	uint64 stride;   // Size of each element in the vector (in bytes)
} VectorHeader;

static void* vector_create(uint64 initial_capacity, uint64 stride)
{
	uint64 header_size       = sizeof(VectorHeader);
	uint64 memory_block_size = initial_capacity * stride;

	void* memory = calloc(header_size + memory_block_size, sizeof(int8));

	VectorHeader* header = memory;
	header->length       = 0;
	header->capacity     = initial_capacity;
	header->stride       = stride;

	return (uint8*)memory + header_size;
}

static void vector_destroy(void* vector)
{
	uint64 header_size   = sizeof(VectorHeader);
	VectorHeader* header = (VectorHeader*)((uint8*)vector - header_size);

	free(header);
}

static void* vector_push(void* vector, const void* value)
{
	uint64 header_size   = sizeof(VectorHeader);
	VectorHeader* header = (VectorHeader*)((uint8*)vector - header_size);

	uint64 address = (uint64)vector;
	address += (header->length * header->stride);

	memcpy((void*)address, value, header->stride);

	header->length++;

	return vector;
}

static uint64 vector_capacity(void* vector)
{
	uint64 header_size   = sizeof(VectorHeader);
	VectorHeader* header = (VectorHeader*)((uint8*)vector - header_size);

	return header->capacity;
}

static uint64 vector_length(void* vector)
{
	uint64 header_size   = sizeof(VectorHeader);
	VectorHeader* header = (VectorHeader*)((uint8*)vector - header_size);

	return header->length;
}

static uint64 vector_stride(void* vector)
{
	uint64 header_size   = sizeof(VectorHeader);
	VectorHeader* header = (VectorHeader*)((uint8*)vector - header_size);

	return header->stride;
}
