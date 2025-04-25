/**
 * @file hash_map.h
 * @author TF
 * @copyright Copyright (c) 2023 TF
 */
#pragma once

/**
 * @brief HashNode is a structure that represents a node in the hash map.
 */
typedef struct _HashNode
{
	char* key;              // Key of the node
	void* value;            // Pointer to the value of the node
	struct _HashNode* next; // Pointer to the next node in the linked list
} HashNode;

/**
 * @brief HashMap is a structure that represents a hash map.
 */
typedef struct _HashMap
{
	uint64 stride;      // Size of each element (in bytes)
	uint64 capacity;    // Number of buckets in the hash map
	HashNode** buckets; // Array of pointers to the hash nodes (buckets)
} HashMap;

/**
 * @brief Create a new hash map with the specified stride and capacity.
 *
 * @param stride Size of each element (in bytes)
 * @param capacity Number of buckets in the hash map
 * @return Newly created hash map.
 */
HashMap hash_map_create(uint64 stride, uint64 capacity);

/**
 * @brief Destroy the hash map and free its memory.
 *
 * @param map A pointer to the hash map to be destroyed.
 */
void hash_map_destroy(HashMap* map);

/**
 * @brief Set a value in the hash map for a given key. (Internal use, use only via macro hash_map_set)
 *
 * @param map The hash map to set the value in
 * @param key The key for the value to be set
 * @param value The value to be set
 */
void _hash_map_set(const HashMap* map, const char* key, const void* value);

/**
 * @brief Set a value in the hash map for a given key.
 *
 * @param map The hash map to set the value in
 * @param key The key for the value to be set
 * @param value The value to be set
 */
#define hash_map_set(map, key, value)             \
	{                                             \
		typeof(value) __temp_value__ = value;     \
		_hash_map_set(map, key, &__temp_value__); \
	}

/**
 * @brief Get a hash node from the hash map for a given key.
 *
 * @param map The hash map to get the value from
 * @param key The key for the value to be retrieved
 * @return HashNode* A pointer to the hash node containing the value
 */
HashNode* hash_map_get(const HashMap* map, const char* key);

/**
 * @brief Get the value from the hash map for a given key.
 *
 * @param map The hash map to get the value from
 * @param key The key for the value to be retrieved
 * @return void* A pointer to the value associated with the key
 */
void* hash_map_get_value(const HashMap* map, const char* key);
