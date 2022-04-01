#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

extern int test_main(int, char **);


static void test_main_no_command(void **state)
{
	char *argv_no_cmd[] = {
		"test_main"
	};
	assert_int_equal(test_main(1, argv_no_cmd), -1);
}

static void test_main_invalid_command(void **state)
{
	char *argv[] = {
		"test_main",
		"invalid"
	};
	assert_int_equal(test_main(2, argv), -1);
}

static void test_main_help(void **state)
{
	char *argv[] = {
		"test_main",
		"help"
	};
	assert_int_equal(test_main(2, argv), -1);
}

int main(int argc, char *argv[])
{
	struct CMUnitTest tests[] = {
		cmocka_unit_test(test_main_no_command),
		cmocka_unit_test(test_main_invalid_command),
		cmocka_unit_test(test_main_help)
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
