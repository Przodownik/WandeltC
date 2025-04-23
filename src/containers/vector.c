#include "vector.h"

void* vector_create(uint64 initial_capacity, uint64 stride)
{
	ASSERT(initial_capacity > 0, "Initial capacity must be greater than 0");
	ASSERT(stride > 0, "Stride must be greater than 0");

	uint64 header_size       = sizeof(VectorHeader);
	uint64 memory_block_size = initial_capacity * stride;

	void* memory = calloc(header_size + memory_block_size, sizeof(int8));

	VectorHeader* header = memory;
	header->length       = 0u;
	header->capacity     = initial_capacity;
	header->stride       = stride;

	return (uint8*)memory + header_size;
}

void vector_destroy(void* vector)
{
	ASSERT(vector != nullptr, "Vector is nullptr");

	uint64 header_size   = sizeof(VectorHeader);
	VectorHeader* header = (VectorHeader*)((uint8*)vector - header_size);

	free(header);
}

void* vector_resize(void* vector)
{
	uint64 header_size   = sizeof(VectorHeader);
	VectorHeader* header = (VectorHeader*)((uint8*)vector - header_size);

	void* new_vector = vector_create(header->capacity * VECTOR_RESIZE_FACTOR, header->stride);

	VectorHeader* new_header = (VectorHeader*)((uint8*)new_vector - header_size);
	new_header->length       = header->length;

	memcpy(new_vector, vector, header->length * header->stride);

	vector_destroy(vector);

	return new_vector;
}

void* vector_push(void* vector, const void* value)
{
	uint64 header_size   = sizeof(VectorHeader);
	VectorHeader* header = (VectorHeader*)((uint8*)vector - header_size);

	if (header->length >= header->capacity)
	{
		vector = vector_resize(vector);
		header = (VectorHeader*)((uint8*)vector - header_size);
	}

	uint64 address = (uint64)vector;
	address += (header->length * header->stride);

	memcpy((void*)address, value, header->stride);

	header->length++;

	return vector;
}

uint64 vector_get_capacity(void* vector)
{
	uint64 header_size   = sizeof(VectorHeader);
	VectorHeader* header = (VectorHeader*)((uint8*)vector - header_size);

	return header->capacity;
}

uint64 vector_get_length(void* vector)
{
	uint64 header_size   = sizeof(VectorHeader);
	VectorHeader* header = (VectorHeader*)((uint8*)vector - header_size);

	return header->length;
}

void vector_set_length(void* vector, uint64 value)
{
	uint64 header_size   = sizeof(VectorHeader);
	VectorHeader* header = (VectorHeader*)((uint8*)vector - header_size);

	header->length = value;
}

uint64 vector_get_stride(void* vector)
{
	uint64 header_size   = sizeof(VectorHeader);
	VectorHeader* header = (VectorHeader*)((uint8*)vector - header_size);

	return header->stride;
}

void vector_clear(void* vector)
{
	vector_set_length(vector, 0);
}
