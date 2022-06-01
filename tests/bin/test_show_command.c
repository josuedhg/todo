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
	.driver = &mock_todo_driver,
};

static struct command_descriptor desc = {SHOW_COMMAND_ID, "show", "show [task_id]", "Show task details" };

static struct command command = {
	.descriptor = &desc,
	.todo = &todo,
	.log = &test_logger,
};

static void test_show_command_no_param(void **state)
{
	(void)state; /* unused */

	expect_string(mock_log_function, report_string, "Usage: show [task_id] \n");
	assert_int_equal(command_handle(&command), -1);
}

static void test_show_command_invalid_param(void **state)
{
	(void)state; /* unused */
	char *params[] = {"show", "invalid"};

	command.argv = params;
	command.argc = 2;

	expect_string(mock_log_function, report_string, "Invalid task id format: invalid\n");
	assert_int_equal(command_handle(&command), -1);
}

static void test_show_command_task_not_found(void **state)
{
	(void)state; /* unused */
	char *params[] = {"show", "1"};

	command.argv = params;
	command.argc = 2;

	will_return(mock_todo_get_task, NULL);
	expect_string(mock_log_function, report_string, "Error: Unable to find task with id 1.\n");
	assert_int_equal(command_handle(&command), -1);
}

static void test_show_command_task_found(void **state)
{
	(void)state; /* unused */
	char *params[] = {"show", "1"};
	struct task *task = create_task("name", "project", TASK_PRIORITY_LOW);

	// setup todo
	will_return(mock_todo_get_task, task);

	// setup stdout
	expect_string(mock_log_function, report_string, "Task 1: name\n"
							"Project project\n"
							"Status open\n");
	command.argv = params;
	command.argc = 2;

	// run command
	assert_int_equal(command_handle(&command), 0);

	// cleanup
	todo.task_list[0] = NULL;
	todo.task_counter = 0;
	destroy_task(&task);
}

int main(int argc, char *argv[])
{
	struct CMUnitTest tests[] = {
		cmocka_unit_test(test_show_command_no_param),
		cmocka_unit_test(test_show_command_invalid_param),
		cmocka_unit_test(test_show_command_task_not_found),
		cmocka_unit_test(test_show_command_task_found),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
