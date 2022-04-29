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

static struct command_descriptor desc = {DELETE_COMMAND_ID, "delete", "delete [task id]", "Delete a task" };

static struct command command = {
	.descriptor = &desc,
	.todo = &todo,
	.log = &logger,
};

extern int test_main(int, char **);

static void test_delete_command_listed_in_help(void **state)
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
	assert_non_null(strstr(buffer, "delete: Delete a task"));
	free(buffer);
}

static void test_delete_command_no_param(void **state)
{
	(void)state; /* unused */

	expect_string(mock_log_function, report_string, "Usage: delete [task id] \n");
	assert_int_equal(command_handle(&command), -1);
}

static void test_delete_command_invalid_param(void **state)
{
	(void)state; /* unused */
	char *params[] = {"delete", "invalid"};

	command.argv = params;
	command.argc = 2;

	expect_string(mock_log_function, report_string, "Invalid task id format: invalid\n");
	assert_int_equal(command_handle(&command), -1);
}

static void test_delete_command_task_not_found(void **state)
{
	(void)state; /* unused */
	char *params[] = {"delete", "1"};

	command.argv = params;
	command.argc = 2;

	expect_string(mock_log_function, report_string, "Error: Unable to find task with id 1.\n");
	assert_int_equal(command_handle(&command), -1);
}

static void test_delete_command_cannot_save_task(void **state)
{
	(void)state; /* unused */
	char *params[] = {"delete", "1"};
	struct task *task = create_task("name", "project", TASK_PRIORITY_LOW);

	todo.task_list[0] = task;
	todo.task_counter = 1;

	command.argv = params;
	command.argc = 2;

	instrument_stderr();
	will_return(__wrap_todo_save_tasks, -1);
	expect_string(mock_log_function, report_string, "Error: Unable to save tasks\n");
	assert_int_equal(command_handle(&command), -1);

	todo.task_list[0] = NULL;
	todo.task_counter = 0;
	destroy_task(&task);
}

static void test_delete_command_success(void **state)
{
	(void)state; /* unused */
	char *params[] = {"delete", "1"};
	struct task *task = create_task("name", "project", TASK_PRIORITY_LOW);

	todo.task_list[0] = task;
	todo.task_counter = 1;

	command.argv = params;
	command.argc = 2;

	will_return(__wrap_todo_save_tasks, 0);
	assert_int_equal(command_handle(&command), 0);
	todo.task_list[0] = NULL;
	todo.task_counter = 0;
	destroy_task(&task);
}

int main(int argc, char *argv[])
{
	struct CMUnitTest tests[] = {
		cmocka_unit_test(test_delete_command_listed_in_help),
		cmocka_unit_test(test_delete_command_no_param),
		cmocka_unit_test(test_delete_command_invalid_param),
		cmocka_unit_test(test_delete_command_task_not_found),
		cmocka_unit_test(test_delete_command_cannot_save_task),
		cmocka_unit_test(test_delete_command_success),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}