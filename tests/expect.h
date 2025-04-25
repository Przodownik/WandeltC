/**
 * @file expect.h
 * @author TF
 * @copyright Copyright (c) 2025 TF
 */
#pragma once

#include "test_manager.h"

/**
 * @brief Macro to check if the actual value is equal to the expected value.
 * If not, it logs an error message and returns TEST_RESULT_FAIL.
 *
 * @param expected The expected value.
 * @param actual The actual value.
 */
#define expect_should_be(expected, actual)                                                               \
	if (actual != expected)                                                                              \
	{                                                                                                    \
		ERROR("--> Expected %lld, but got: %lld. File: %s:%d.\n", expected, actual, __FILE__, __LINE__); \
		return TEST_RESULT_FAIL;                                                                         \
	}

/**
 * @brief Macro to check if the actual value is not equal to the expected value.
 * If they are equal, it logs an error message and returns TEST_RESULT_FAIL.
 *
 * @param expected The expected value.
 * @param actual The actual value.
 */
#define expect_should_not_be(expected, actual)                                                                    \
	if (actual == expected)                                                                                       \
	{                                                                                                             \
		ERROR("--> Expected %d != %d, but they are equal. File: %s:%d.\n", expected, actual, __FILE__, __LINE__); \
		return TEST_RESULT_FAIL;                                                                                  \
	}

/**
 * @brief Macro to check if the actual value is greater than the expected value.
 * If not, it logs an error message and returns TEST_RESULT_FAIL.
 *
 * @param expected The expected value.
 * @param actual The actual value.
 */
#define expect_to_be_true(actual)                                                       \
	if (actual != true)                                                                 \
	{                                                                                   \
		ERROR("--> Expected true, but got: false. File: %s:%d.\n", __FILE__, __LINE__); \
		return TEST_RESULT_FAIL;                                                        \
	}

/**
 * @brief Macro to check if the actual value is less than the expected value.
 * If not, it logs an error message and returns TEST_RESULT_FAIL.
 *
 * @param expected The expected value.
 * @param actual The actual value.
 */
#define expect_to_be_false(actual)                                                      \
	if (actual != false)                                                                \
	{                                                                                   \
		ERROR("--> Expected false, but got: true. File: %s:%d.\n", __FILE__, __LINE__); \
		return TEST_RESULT_FAIL;                                                        \
	}
