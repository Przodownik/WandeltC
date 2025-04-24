/**
 * @file vector.h
 * @author TF
 * @copyright Copyright (c) 2023 TF
 */
#pragma once

/**
 * @struct VectorHeader
 * @brief Structure representing the header of a vector.
 */
typedef struct _VectorHeader
{
	uint64 length;   // Number of elements in the vector
	uint64 capacity; // Capacity of the vector (number of elements it can hold)
	uint64 stride;   // Size of each element in the vector (in bytes)
} VectorHeader;

/**
 * @brief Resize factor for the vector.
 * This factor determines how much the vector's capacity is increased when it needs to be resized.
 */
#define VECTOR_RESIZE_FACTOR 2

/**
 * @brief Create a new vector with the specified initial capacity and stride.
 * @param initial_capacity The initial capacity of the vector.
 * @param stride The size of each element in the vector (in bytes).
 * @return A pointer to the newly created vector.
 */
void* vector_create(uint64 initial_capacity, uint64 stride);

/**
 * @brief Destroy the vector and free its memory.
 *
 * @param vector A pointer to the vector to be destroyed.
 */
void vector_destroy(void* vector);

/**
 * @brief Resize the vector to accommodate more elements.
 *
 * @param vector A pointer to the vector to be resized.
 * @return A pointer to the resized vector.
 */
void* vector_resize(void* vector);

/**
 * @brief Push a new value onto the vector. (Internal use, use only via macro vector_push)
 *
 * @param vector A pointer to the vector to which the value will be pushed.
 * @param value A pointer to the value to be pushed onto the vector.
 * @return A pointer to the updated vector.
 */
void* _vector_push(void* vector, const void* value);

/**
 * @brief Push a new value onto the vector.
 *
 * @param vector A pointer to the vector to which the value will be pushed.
 * @param value A pointer to the value to be pushed onto the vector.
 */
#define vector_push(vector, value)                                            \
	{                                                                         \
		typeof(value) __temp_value__ = value;                                 \
		vector                       = _vector_push(vector, &__temp_value__); \
	}

/**
 * @brief Get the capacity of the vector. (number of elements it can hold)
 *
 * @param vector A pointer to the vector.
 * @return uint64 The capacity of the vector.
 */
uint64 vector_get_capacity(void* vector);

/**
 * @brief Get the length of the vector. (number of elements currently in the vector)
 *
 * @param vector A pointer to the vector.
 * @return uint64 The length of the vector.
 */
uint64 vector_get_length(void* vector);

/**
 * @brief Set the length of the vector. (number of elements currently in the vector)
 *
 * @param vector A pointer to the vector.
 * @param value The new length of the vector.
 * @warning This function does not resize the vector; it only updates the length field.
 */
void vector_set_length(void* vector, uint64 value);

/**
 * @brief Get the stride of the vector. (size of each element in the vector)
 *
 * @param vector A pointer to the vector.
 * @return uint64 The stride of the vector.
 */
uint64 vector_get_stride(void* vector);

/**
 * @brief Clear the vector by resetting its length to zero.
 *
 * @param vector
 */
void vector_clear(void* vector);
