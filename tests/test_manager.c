#include "test_manager.h"

#include <containers/vector.h>

typedef struct test_entry
{
	PFN_test func;
	const char* desc;
} test_entry;

static test_entry* tests = nullptr;

void test_manager_init(void)
{
	tests = vector_create(10, sizeof(test_entry));
}

void test_manager_register_test(PFN_test fn, const char* desc)
{
	test_entry new_entry;
	new_entry.func = fn;
	new_entry.desc = desc;

	vector_push(tests, new_entry);
}

void test_manager_run_tests(void)
{
	uint32 passed  = 0;
	uint32 failed  = 0;
	uint32 skipped = 0;

	for (uint64 i = 0; i < vector_get_length(tests); ++i)
	{
		test_result result = tests[i].func();

		switch (result)
		{
		case TEST_RESULT_PASS:
			passed++;
			TRACE(ANSI_COLOR_GREEN "Test %s passed\n" ANSI_COLOR_RESET, tests[i].desc);
			break;
		case TEST_RESULT_SKIP:
			skipped++;
			TRACE(ANSI_COLOR_YELLOW "Test %s skipped\n" ANSI_COLOR_RESET, tests[i].desc);
			break;
		case TEST_RESULT_FAIL:
			failed++;
			TRACE(ANSI_COLOR_RED "Test %s failed\n" ANSI_COLOR_RESET, tests[i].desc);
			break;
		}
	}

	TRACE("Tests completed: %u passed, %u failed, %u skipped\n", passed, failed, skipped);
	TRACE("Total tests run: %u\n", passed + failed + skipped);
	TRACE("Test results: %s\n", (failed == 0) ? "All tests passed!" : "Some tests failed!");
}

void test_manager_shutdown(void)
{
	vector_destroy(tests);

	tests = nullptr;
}
