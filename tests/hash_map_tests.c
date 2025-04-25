#include "hash_map_tests.h"

#include "expect.h"
#include "test_manager.h"

#include "containers/hash_map.h"

static TestResult hash_map_simple_type_tests(void)
{
	HashMap map = hash_map_create(sizeof(uint64), 10);

	hash_map_set(&map, "key1", 12llu);

	uint64 x = 50llu;
	hash_map_set(&map, "key2", x);

	uint64* r = hash_map_get_value(&map, "key1");
	expect_should_be(12llu, *r);

	uint64* t = hash_map_get_value(&map, "key2");
	expect_should_be(x, *t);

	hash_map_destroy(&map);

	return TEST_RESULT_PASS;
}

static TestResult hash_map_struct_tests(void)
{
	typedef struct
	{
		int x;
		int y;
	} Point;

	HashMap map = hash_map_create(sizeof(Point), 10);

	Point p1 = {0};
	p1.x     = 12;
	p1.y     = 13;

	hash_map_set(&map, "key1", p1);

	Point* p = hash_map_get_value(&map, "key1");
	expect_should_be((uint64)p1.x, (uint64)p->x);
	expect_should_be((uint64)p1.y, (uint64)p->y);

	hash_map_destroy(&map);

	return TEST_RESULT_PASS;
}

static TestResult hash_map_string_tests(void)
{
	const char* str1 = "Hello";
	const char* str2 = "world!";

	HashMap map = hash_map_create(sizeof(char*), 10);

	hash_map_set(&map, "key1", str1);
	hash_map_set(&map, "key2", str2);

	const char** out1 = hash_map_get_value(&map, "key1");
	const char** out2 = hash_map_get_value(&map, "key2");

	expect_to_be_true(strcmp(*out1, "Hello") == 0);
	expect_to_be_true(strcmp(*out2, "world!") == 0);

	hash_map_destroy(&map);

	return TEST_RESULT_PASS;
}

static TestResult hash_map_pointer_tests(void)
{
	int64 i1 = 12;

	HashMap map = hash_map_create(sizeof(int64*), 10);

	hash_map_set(&map, "key1", &i1);

	int64** p = hash_map_get_value(&map, "key1");
	expect_should_be(12llu, **p);

	hash_map_destroy(&map);

	return TEST_RESULT_PASS;
}

void hash_map_register_tests(void)
{
	test_manager_register_test(hash_map_simple_type_tests, "Basic hash map test using simple types");
	test_manager_register_test(hash_map_struct_tests, "Basic hash map test using structs");
	test_manager_register_test(hash_map_string_tests, "Basic hash map test using strings");
	test_manager_register_test(hash_map_pointer_tests, "Basic hash map test using pointers");
}
