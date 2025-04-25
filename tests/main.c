#include "test_manager.h"

#include "containers/hash_map_tests.h"
#include "containers/vector_tests.h"

void main(void)
{
	test_manager_init();

	vector_register_tests();
	hash_map_register_tests();

	test_manager_run_tests();

	test_manager_shutdown();
}
