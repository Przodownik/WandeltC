#include "test_manager.h"

#include "vector_tests.h"

void main(void)
{
	test_manager_init();

	vector_register_tests();

	test_manager_run_tests();

	test_manager_shutdown();
}
