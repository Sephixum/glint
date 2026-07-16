#include "g_test_common.h"

// Define global test registry
test_entry global_test_entries[GLINT_TEST_MAX_ENTRIES];
int global_test_entry_count = 0;

// Define global test stats
test_state global_test_stats = {0};

int main(int argc, char* argv[])
{
	char const* filter = (argc > 1) ? argv[1] : 0;
	return test_run_all(filter);
}
