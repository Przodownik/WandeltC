#include "vector_tests.h"

#include "expect.h"
#include "test_manager.h"

static test_result vector_simple_type_tests(void)
{
	int* intVector = vector_create(5, sizeof(int));

	expect_should_be(5llu, vector_get_capacity(intVector));
	expect_should_be(4llu, vector_get_stride(intVector));
	expect_should_be(0llu, vector_get_length(intVector));

	intVector[0] = 12;
	intVector[1] = 13;
	intVector[2] = 14;
	intVector[3] = 15;
	intVector[4] = 16;

	expect_should_be(12llu, (uint64)intVector[0]);
	expect_should_be(13llu, (uint64)intVector[1]);
	expect_should_be(14llu, (uint64)intVector[2]);
	expect_should_be(15llu, (uint64)intVector[3]);
	expect_should_be(16llu, (uint64)intVector[4]);

	expect_should_be(0llu, vector_get_length(intVector));
	vector_set_length(intVector, 5);
	expect_should_be(5llu, vector_get_length(intVector));
	expect_should_be(5llu, vector_get_capacity(intVector));

	vector_destroy(intVector);

	return TEST_RESULT_PASS;
}

static test_result vector_struct_tests(void)
{
	typedef struct
	{
		int x;
		int y;
	} Point;

	Point* pointVector = vector_create(5, sizeof(Point));

	expect_should_be(5llu, vector_get_capacity(pointVector));
	expect_should_be(8llu, vector_get_stride(pointVector));
	expect_should_be(0llu, vector_get_length(pointVector));

	pointVector[0].x = 1;
	pointVector[0].y = 2;
	pointVector[1].x = 3;
	pointVector[1].y = 4;

	expect_should_be(1llu, (uint64)pointVector[0].x);
	expect_should_be(2llu, (uint64)pointVector[0].y);
	expect_should_be(3llu, (uint64)pointVector[1].x);
	expect_should_be(4llu, (uint64)pointVector[1].y);

	vector_destroy(pointVector);

	return TEST_RESULT_PASS;
}

static test_result vector_string_tests(void)
{
	char** stringVector = vector_create(5, sizeof(char*));

	const char* str1 = "Hello";
	const char* str2 = "world!";

	stringVector = vector_push(stringVector, str1);
	stringVector = vector_push(stringVector, str2);

	expect_should_be(2llu, vector_get_length(stringVector));

	expect_to_be_true(strcmp(stringVector, "Hello") == 0);
	expect_to_be_true(strcmp(stringVector + 1, "world!") == 0);

	return TEST_RESULT_PASS;
}

static test_result vector_pointer_tests(void)
{
	int** intVector = vector_create(5, sizeof(int*));

	int a = 1;
	int b = 2;
	int c = 3;

	intVector[0] = &a;
	intVector[1] = &b;
	intVector[2] = &c;

	expect_should_be(1llu, (uint64)*intVector[0]);
	expect_should_be(2llu, (uint64)*intVector[1]);
	expect_should_be(3llu, (uint64)*intVector[2]);

	vector_destroy(intVector);

	return TEST_RESULT_PASS;
}

static test_result vector_resize_tests(void)
{
	int* intVector = vector_create(2, sizeof(int));

	int val = 1;

	intVector = vector_push(intVector, &val);
	intVector = vector_push(intVector, &val);
	intVector = vector_push(intVector, &val);

	expect_should_be(3llu, vector_get_length(intVector));
	expect_should_be(4llu, vector_get_capacity(intVector));

	intVector = vector_push(intVector, &val);
	intVector = vector_push(intVector, &val);
	intVector = vector_push(intVector, &val);

	expect_should_be(6llu, vector_get_length(intVector));
	expect_should_be(8llu, vector_get_capacity(intVector));

	return TEST_RESULT_PASS;
}

void vector_register_tests(void)
{
	test_manager_register_test(vector_simple_type_tests, "Basic vector tests using simple type storage");
	test_manager_register_test(vector_struct_tests, "Basic vector tests using struct type storage");
	test_manager_register_test(vector_string_tests, "Basic vector tests using string type storage");
	test_manager_register_test(vector_pointer_tests, "Basic vector tests using pointer type storage");
	test_manager_register_test(vector_resize_tests, "Basic vector tests using resize type storage");
}
