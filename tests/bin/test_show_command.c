#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdlib.h>
#include <string.h>

#include "todo.h"
#include "commands.h"
#include "instrument.h"

struct todo todo = {
	.task_counter = 0,
};

extern int test_main(int, char **);

static void test_show_command_listed_in_help(void **state)
{
	char *argv_no_cmd[] = {
		"test_main",
		"help",
	};

	char *buffer = NULL;
	size_t buffer_size = 0;

	instrument_stderr();
	assert_int_equal(test_main(1, argv_no_cmd), -1);
	buffer_size = get_stderr_buffer(&buffer);
	deinstrument_stderr();

	assert_int_not_equal(buffer_size, 0);
	assert_non_null(strstr(buffer, "show: Show task details\n"));
	free(buffer);
}

static void test_show_command_no_param(void **state)
{
	(void)state; /* unused */
	char *buffer = NULL;
	size_t buffer_size = 0;

	instrument_stderr();
	assert_int_equal(show_command.command_handle(0, NULL), -1);
	buffer_size = get_stderr_buffer(&buffer);
	deinstrument_stderr();

	assert_int_not_equal(buffer_size, 0);
	assert_non_null(strstr(buffer, "Usage: show [task_id] \n"));
	free(buffer);
}

static void test_show_command_invalid_param(void **state)
{
	(void)state; /* unused */
	char *params[] = {"show", "invalid"};
	char *buffer = NULL;
	size_t buffer_size = 0;

	instrument_stderr();
	assert_int_equal(show_command.command_handle(2, params), -1);
	buffer_size = get_stderr_buffer(&buffer);
	deinstrument_stderr();

	assert_int_not_equal(buffer_size, 0);
	assert_non_null(strstr(buffer, "Invalid task id format: invalid\n"));
	free(buffer);
}

static void test_show_command_cannot_access_file(void **state)
{
	(void)state; /* unused */
	char *params[] = {"show", "1"};
	char *buffer = NULL;
	size_t buffer_size = 0;

	instrument_stderr();
	will_return(__wrap_create_todotxt, NULL);
	assert_int_equal(show_command.command_handle(2, params), -1);
	buffer_size = get_stderr_buffer(&buffer);
	deinstrument_stderr();

	assert_int_not_equal(buffer_size, 0);
	assert_non_null(strstr(buffer, "Error: Unable to read "));
	free(buffer);
}

static void test_show_command_cannot_read_file(void **state)
{
	(void)state; /* unused */
	char *params[] = {"show", "1"};
	char *buffer = NULL;
	size_t buffer_size = 0;

	instrument_stderr();
	will_return(__wrap_create_todotxt, &todo);
	will_return(__wrap_todo_load_tasks, -1);
	assert_int_equal(show_command.command_handle(2, params), -1);
	buffer_size = get_stderr_buffer(&buffer);
	deinstrument_stderr();

	assert_int_not_equal(buffer_size, 0);
	assert_non_null(strstr(buffer, "Error: Unable to load tasks from "));
	free(buffer);
}

static void test_show_command_task_not_found(void **state)
{
	(void)state; /* unused */
	char *params[] = {"show", "1"};
	char *buffer = NULL;
	size_t buffer_size = 0;

	instrument_stderr();
	will_return(__wrap_create_todotxt, &todo);
	will_return(__wrap_todo_load_tasks, 0);
	assert_int_equal(show_command.command_handle(2, params), -1);
	buffer_size = get_stderr_buffer(&buffer);
	deinstrument_stderr();

	assert_int_not_equal(buffer_size, 0);
	assert_non_null(strstr(buffer, "Error: Unable to find task with id 1.\n"));
	free(buffer);
}

static void test_show_command_task_found(void **state)
{
	(void)state; /* unused */
	char *params[] = {"show", "1"};
	struct task *task = create_task("name", "project", TASK_PRIORITY_LOW);
	char *buffer = NULL;
	size_t buffer_size = 0;

	// setup todo
	todo.task_list[0] = task;
	todo.task_counter = 1;

	// setup stdout
	instrument_stdout();

	// prepare returns
	will_return(__wrap_create_todotxt, &todo);
	will_return(__wrap_todo_load_tasks, 0);

	// run command
	assert_int_equal(show_command.command_handle(2, params), 0);

	// get stdout
	buffer_size = get_stdout_buffer(&buffer);
	deinstrument_stdout();

	// check results
	assert_int_not_equal(buffer_size, 0);
	assert_non_null(strstr(buffer, "Task 1: name\n"
					"Project project\n"
					"Status open\n"));

	// cleanup
	todo.task_list[0] = NULL;
	todo.task_counter = 0;
	destroy_task(&task);
	free(buffer);
}

int main(int argc, char *argv[])
{
	struct CMUnitTest tests[] = {
		cmocka_unit_test(test_show_command_listed_in_help),
		cmocka_unit_test(test_show_command_no_param),
		cmocka_unit_test(test_show_command_invalid_param),
		cmocka_unit_test(test_show_command_cannot_access_file),
		cmocka_unit_test(test_show_command_cannot_read_file),
		cmocka_unit_test(test_show_command_task_not_found),
		cmocka_unit_test(test_show_command_task_found),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
