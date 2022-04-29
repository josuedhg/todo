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
	.log = &test_logger,
};

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
		cmocka_unit_test(test_delete_command_no_param),
		cmocka_unit_test(test_delete_command_invalid_param),
		cmocka_unit_test(test_delete_command_task_not_found),
		cmocka_unit_test(test_delete_command_cannot_save_task),
		cmocka_unit_test(test_delete_command_success),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
