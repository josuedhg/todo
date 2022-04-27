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

static struct command_descriptor desc = {LIST_COMMAND_ID, "list", "list", "List all available commands" };

static struct command command = {
	.descriptor = &desc,
	.todo = &todo,
};

extern int test_main(int, char **);

static void test_list_command_listed_in_help(void **state)
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
	assert_non_null(strstr(buffer, "list: List all available commands\n"));
	free(buffer);
}

static void test_list_command_no_tasks(void **state)
{
	(void)state; /* unused */
	char *buffer = NULL;
	size_t buffer_size = 0;

	instrument_stdout();
	assert_int_equal(command_handle(&command), 0);
	buffer_size = get_stdout_buffer(&buffer);
	deinstrument_stdout();

	assert_int_equal(buffer_size, 0);
}

static void test_list_command_one_task(void **state)
{
	(void)state; /* unused */
	char *buffer = NULL;
	size_t buffer_size = 0;
	struct task *task = create_task("name", "project", TASK_PRIORITY_LOW);

	// setup todo
	todo.task_list[0] = task;
	todo.task_counter = 1;

	// setup stdout
	instrument_stdout();

	// run command
	assert_int_equal(command_handle(&command), 0);

	// get stdout
	buffer_size = get_stdout_buffer(&buffer);
	deinstrument_stdout();

	// check results
	assert_int_not_equal(buffer_size, 0);
	assert_non_null(strstr(buffer, "1. name\n"));

	// cleanup
	todo.task_list[0] = NULL;
	todo.task_counter = 0;
	destroy_task(&task);
	free(buffer);
}

static void test_list_command_multiple_tasks(void **state)
{
	(void)state; /* unused */
	char *buffer = NULL;
	size_t buffer_size = 0;
	struct task *task1 = create_task("name1", "project1", TASK_PRIORITY_LOW);
	struct task *task2 = create_task("name2", "project2", TASK_PRIORITY_LOW);

	// setup todo
	todo.task_list[0] = task1;
	todo.task_list[1] = task2;
	todo.task_counter = 2;

	// setup stdout
	instrument_stdout();

	// run command
	assert_int_equal(command_handle(&command), 0);

	// get stdout
	buffer_size = get_stdout_buffer(&buffer);
	deinstrument_stdout();

	// check results
	assert_int_not_equal(buffer_size, 0);
	assert_non_null(strstr(buffer, "1. name1\n"));
	assert_non_null(strstr(buffer, "2. name2\n"));

	// cleanup
	todo.task_list[0] = NULL;
	todo.task_list[1] = NULL;
	todo.task_counter = 0;
	destroy_task(&task1);
	destroy_task(&task2);
	free(buffer);
}


int main(int argc, char *argv[])
{
	struct CMUnitTest tests[] = {
		cmocka_unit_test(test_list_command_listed_in_help),
		cmocka_unit_test(test_list_command_no_tasks),
		cmocka_unit_test(test_list_command_one_task),
		cmocka_unit_test(test_list_command_multiple_tasks),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
