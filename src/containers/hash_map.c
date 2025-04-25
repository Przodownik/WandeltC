#include "hash_map.h"
#include "utils/defines.h"
#include <string.h>

// Check if a number is prime
static bool is_prime(const uint64 n)
{
	// Numbers < 2 are not prime
	if (n < 2)
		return false;

	// 2 and 3 are prime
	if (n <= 3)
		return true;

	// Eliminate multiples of 2 and 3
	if (n % 2 == 0 || n % 3 == 0)
		return false;

	for (uint64 i = 5; i * i <= n; i += 6)
		if (n % i == 0 || n % (i + 2) == 0)
			return false;

	return true;
}

// Find the next prime number >= n
static uint64 round_to_prime_number(uint64 number)
{
	// The smallest prime number
	if (number <= 2)
		return 2;

	// Ensure we start with an odd number for efficiency
	if (number % 2 == 0)
		number++;

	while (!is_prime(number)) number += 2; // Skip even numbers

	return number;
}

HashMap hash_map_create(uint64 stride, uint64 capacity)
{
	ASSERT(stride > 0, "Stride is 0! Cannot create a hash map with 0 stride elements!");
	ASSERT(capacity > 0, "Capacity is 0! Cannot create a hash map with 0 capacity!");

	capacity = round_to_prime_number(capacity); // Ensure capacity is prime for better distribution

	HashMap map;
	map.stride   = stride;
	map.capacity = capacity;
	map.buckets  = calloc(capacity, sizeof(HashNode*));

	ASSERT(map.buckets != NULL, "Failed to allocate memory for hash map buckets!");

	return map;
}

void hash_map_destroy(HashMap* map)
{
	free(map->buckets);
}

static const uint64 prime = 1099511628211ULL; // FNV prime

static uint64 hashed_offset(const char* fragment, uint64 count)
{
	uint64 hash = 14695981039346656037ULL;

	for (uint64 i = 0; i < strlen(fragment); i++)
	{
		hash ^= (uint64)fragment[i];
		hash *= prime;
	}

	return hash % count;
}

void _hash_map_set(const HashMap* map, const char* key, const void* value)
{
	ASSERT(map != nullptr, "Hash map is null!");
	ASSERT(key != nullptr, "Key is null!");
	ASSERT(value != nullptr, "Value is null!");
	ASSERT(strlen(key) > 0, "Key is empty!");

	const uint64 offset = hashed_offset(key, map->capacity);
	HashNode* current   = map->buckets[offset];

	while (current != nullptr)
	{
		if (strcmp(current->key, key) == 0)
		{
			memcpy(current->value, value, map->stride);
			return;
		}

		current = current->next;
	}

	HashNode* new_node = calloc(1, sizeof(HashNode));
	ASSERT(new_node != NULL, "Failed to allocate memory for new hash node!");

	new_node->key = calloc(strlen(key) + 1, sizeof(char));
	ASSERT(new_node->key != NULL, "Failed to allocate memory for new hash node key!");
	strcpy_s(new_node->key, strlen(key) + 1, key);

	new_node->value = calloc(map->stride, sizeof(int8));
	ASSERT(new_node->value != NULL, "Failed to allocate memory for new hash node value!");
	memcpy(new_node->value, value, map->stride);

	map->buckets[offset] = new_node;
}

HashNode* hash_map_get(const HashMap* map, const char* name)
{
	ASSERT(map != nullptr, "Hash map is null!");
	ASSERT(name != nullptr, "Name is null!");
	ASSERT(strlen(name) > 0, "Name is empty!");

	const uint64 offset = hashed_offset(name, map->capacity);
	HashNode* current   = map->buckets[offset];

	while (current != nullptr)
	{
		if (strcmp(current->key, name) == 0)
			return current;

		current = current->next;
	}

	return nullptr;
}

void* hash_map_get_value(const HashMap* map, const char* name)
{
	HashNode* node = hash_map_get(map, name);
	if (node == nullptr)
		return nullptr;

	return node->value;
}
