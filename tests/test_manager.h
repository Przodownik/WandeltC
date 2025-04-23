/**
 * @file test_manager.h
 * @author TF
 * @copyright Copyright (c) 2025 TF
 */
#pragma once

/**
 * @brief Test result enumeration.
 * This enum defines the possible results of a test.
 */
typedef enum
{
	TEST_RESULT_PASS, // Test passed successfully
	TEST_RESULT_SKIP, // Test was skipped
	TEST_RESULT_FAIL, // Test failed
} test_result;

/**
 * @brief Function pointer type for test functions to be registered with the test manager.
 */
typedef test_result (*PFN_test)(void);

/**
 * @brief Initializes the test manager.
 * This function sets up the test manager and prepares it for registering and running tests.
 */
void test_manager_init(void);

/**
 * @brief Registers a test function with the test manager.
 * This function adds a test function to the test manager's list of tests to be run.
 *
 * @param fn The test function to register.
 * @param desc A description of the test.
 */
void test_manager_register_test(PFN_test fn, const char* desc);

/**
 * @brief Runs all registered tests.
 * This function executes all the tests that have been registered with the test manager.
 * It will report the results of each test and provide a summary at the end.
 */
void test_manager_run_tests(void);

/**
 * @brief Shuts down the test manager.
 * This function cleans up any resources used by the test manager and prepares it for termination.
 */
void test_manager_shutdown(void);
