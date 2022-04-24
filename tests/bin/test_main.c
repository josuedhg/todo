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

static void assert_help_output(char *buffer)
{
	assert_non_null(strstr(buffer, "Usage: test_main <command>\n"));
	assert_non_null(strstr(buffer, "Available commands:\n"));
	assert_non_null(strstr(buffer, "help: Display this help message\n"));
}

static void test_main_no_command(void **state)
{
	char *argv_no_cmd[] = {
		"test_main"
	};

	char *buffer = NULL;
	size_t buffer_size = 0;

	instrument_stderr();
	assert_int_equal(test_main(1, argv_no_cmd), -1);
	buffer_size = get_stderr_buffer(&buffer);
	deinstrument_stderr();

	assert_int_not_equal(buffer_size, 0);
	assert_help_output(buffer);
	free(buffer);
}

static void test_main_invalid_command(void **state)
{
	char *argv[] = {
		"test_main",
		"invalid"
	};
	char *buffer = NULL;
	size_t buffer_size = 0;

	instrument_stderr();
	assert_int_equal(test_main(2, argv), -1);
	buffer_size = get_stderr_buffer(&buffer);
	deinstrument_stderr();

	assert_int_not_equal(buffer_size, 0);
	assert_non_null(strstr(buffer, "Unknown command: invalid\n"));
	assert_help_output(buffer);
	free(buffer);
}

static void test_main_command_fail_create_todo(void **state)
{
	char *argv[] = {
		"test_main",
		"list"
	};
	char *buffer = NULL;
	size_t buffer_size = 0;
	struct todo todo;

	// prepare returns
	will_return(__wrap_create_todotxt, NULL);

	instrument_stderr();
	assert_int_equal(test_main(2, argv), -1);
	buffer_size = get_stderr_buffer(&buffer);
	deinstrument_stderr();

	assert_int_not_equal(buffer_size, 0);
	assert_non_null(strstr(buffer, "Error: Unable to read"));
	free(buffer);
}

static void test_main_command_fail_loading_tasks(void **state)
{
	char *argv[] = {
		"test_main",
		"list"
	};
	char *buffer = NULL;
	size_t buffer_size = 0;
	struct todo todo;

	// prepare returns
	will_return(__wrap_create_todotxt, &todo);
	will_return(__wrap_todo_load_tasks, -1);

	instrument_stderr();
	assert_int_equal(test_main(2, argv), -1);
	buffer_size = get_stderr_buffer(&buffer);
	deinstrument_stderr();

	assert_int_not_equal(buffer_size, 0);
	assert_non_null(strstr(buffer, "Error: Unable to load tasks from"));
	free(buffer);
}

static void test_main_command(void **state)
{
	char *argv[] = {
		"test_main",
		"list"
	};
	char *buffer = NULL;
	size_t buffer_size = 0;
	struct todo todo;
	struct task *task1 = create_task("name1", "project1", TASK_PRIORITY_LOW);
	struct task *task2 = create_task("name2", "project2", TASK_PRIORITY_LOW);

	// setup todo
	todo.task_list[0] = task1;
	todo.task_list[1] = task2;
	todo.task_counter = 2;

	// prepare returns
	will_return(__wrap_create_todotxt, &todo);
	will_return(__wrap_todo_load_tasks, 0);

	instrument_stdout();
	assert_int_equal(test_main(2, argv), 0);
	buffer_size = get_stdout_buffer(&buffer);
	deinstrument_stdout();

	assert_int_not_equal(buffer_size, 0);
	free(buffer);
}

int main(int argc, char *argv[])
{
	struct CMUnitTest tests[] = {
		cmocka_unit_test(test_main_no_command),
		cmocka_unit_test(test_main_invalid_command),
		cmocka_unit_test(test_main_command_fail_create_todo),
		cmocka_unit_test(test_main_command_fail_loading_tasks),
		cmocka_unit_test(test_main_command)
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
