#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdlib.h>

#include "instrument.h"
#include "todotxt.h"

extern int test_main(int, char **);

extern struct log logger;

int setup(void **state)
{
	(void)state;
	logger.error = mock_log_function;
	logger.notify = mock_log_function;
	return 0;
}


void check_help(void)
{
	expect_string(mock_log_function, report_string, "Usage: test_main <command>\n"
							"Available commands:\n" \
							"\thelp: Display this help message\n");
	expect_string(mock_log_function, report_string, "\tadd: Add a new task\n");
	expect_string(mock_log_function, report_string, "\tlist: List all available commands\n");
	expect_string(mock_log_function, report_string, "\tshow: Show task details\n");
	expect_string(mock_log_function, report_string, "\tdelete: Delete a task\n");
	expect_string(mock_log_function, report_string, "\tdone: Set task as done\n");
	expect_string(mock_log_function, report_string, "\treopen: Set task as open\n");
}

static void test_main_no_command(void **state)
{
	char *argv_no_cmd[] = {
		"test_main"
	};

	check_help();
	assert_int_equal(test_main(1, argv_no_cmd), -1);
}

static void test_main_invalid_command(void **state)
{
	char *argv[] = {
		"test_main",
		"invalid"
	};

	expect_string(mock_log_function, report_string, "Unknown command: invalid\n");
	check_help();
	assert_int_equal(test_main(2, argv), -1);
}

static void test_main_command_fail_create_todo(void **state)
{
	char *argv[] = {
		"test_main",
		"list"
	};

	// prepare returns
	will_return(__wrap_create_todotxt, NULL);

	char *message = "Error: Unable to read";
	expect_memory(mock_log_function, report_string, message, strlen(message));
	assert_int_equal(test_main(2, argv), -1);
}

static void test_main_command_fail_loading_tasks(void **state)
{
	char *argv[] = {
		"test_main",
		"list"
	};
	struct todo todo;

	// prepare returns
	will_return(__wrap_create_todotxt, &todo);
	will_return(__wrap_todo_load_tasks, -1);

	char *message = "Error: Unable to load tasks from";
	expect_memory(mock_log_function, report_string, message, strlen(message));
	assert_int_equal(test_main(2, argv), -1);
}

static void test_main_command(void **state)
{
	char *argv[] = {
		"test_main",
		"list"
	};
	struct todo todo = { 0 };
	// prepare returns
	will_return(__wrap_create_todotxt, &todo);
	will_return(__wrap_todo_load_tasks, 0);

	assert_int_equal(test_main(2, argv), 0);
}

extern void notify(const char *fmt, ...);
extern void error(const char *fmt, ...);
static void test_check_log_functions(void **state)
{
	expect_string(__wrap_vfprintf, report_string, "test other str");
	notify("test %s", "other str");

	expect_string(__wrap_vfprintf, report_string, "test other str");
	error("test %s", "other str");
}

int main(int argc, char *argv[])
{
	struct CMUnitTest tests[] = {
		cmocka_unit_test(test_main_no_command),
		cmocka_unit_test(test_main_invalid_command),
		cmocka_unit_test(test_main_command_fail_create_todo),
		cmocka_unit_test(test_main_command_fail_loading_tasks),
		cmocka_unit_test(test_main_command),
		cmocka_unit_test(test_check_log_functions),
	};

	return cmocka_run_group_tests(tests, setup, NULL);
}
