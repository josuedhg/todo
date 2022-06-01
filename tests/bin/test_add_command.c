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

static struct command_descriptor desc = {ADD_COMMAND_ID, "add", "add <task description>", "Add a new task" };

static struct command command = {
	.descriptor = &desc,
	.todo = &todo,
	.log = &test_logger,
};

static void test_add_command_no_param(void **state)
{
	(void)state; /* unused */

	expect_string(mock_log_function, report_string, "Usage: add <task description> \n");
	assert_int_equal(command_handle(&command), -1);
}

static void test_add_command_cannot_create_task(void **state)
{
	(void)state; /* unused */
	char *params[] = {"add", "task"};

	command.argv = params;
	command.argc = 2;

	will_return(__wrap_create_new_task, NULL);
	expect_string(mock_log_function, report_string, "Error: Unable to create new task: task\n");
	assert_int_equal(command_handle(&command), -1);
}

extern struct task *__real_create_new_task(const char *, const char *, int);

static void test_add_command_cannot_save_task(void **state)
{
	(void)state; /* unused */
	char *params[] = {"add", "task"};
	struct task *task = __real_create_new_task("name", "project", TASK_PRIORITY_LOW);

	command.argv = params;
	command.argc = 2;

	will_return(__wrap_create_new_task, task);
	expect_string(__wrap_create_new_task, desc, "task");
	expect_value(__wrap_create_new_task, project, NULL);
	expect_value(__wrap_create_new_task, priority, TASK_PRIORITY_LOW);
	will_return(mock_todo_save_tasks, -1);
	expect_string(mock_log_function, report_string, "Error: Unable to save tasks\n");
	assert_int_equal(command_handle(&command), -1);
}

static void test_add_command_success(void **state)
{
	(void)state; /* unused */
	char *params[] = {"add", "task"};
	struct task *task = __real_create_new_task("name", "project", TASK_PRIORITY_LOW);

	command.argv = params;
	command.argc = 2;

	will_return(__wrap_create_new_task, (struct task *)&task);
	expect_string(__wrap_create_new_task, desc, "task");
	expect_value(__wrap_create_new_task, project, NULL);
	expect_value(__wrap_create_new_task, priority, TASK_PRIORITY_LOW);
	will_return(mock_todo_save_tasks, 0);
	assert_int_equal(command_handle(&command), 0);

	destroy_task(&task);
}

static void test_add_command_success_multi_word(void **state)
{
	(void)state; /* unused */
	char *params[] = {"add", "task", "with", "spaces"};
	int fake_task = 1;
	char *buffer = NULL;
	size_t buffer_size = 0;

	command.argv = params;
	command.argc = 4;

	will_return(__wrap_create_new_task, (struct task *)&fake_task);
	expect_string(__wrap_create_new_task, desc, "task with spaces");
	expect_value(__wrap_create_new_task, project, NULL);
	expect_value(__wrap_create_new_task, priority, TASK_PRIORITY_LOW);
	will_return(mock_todo_save_tasks, 0);
	assert_int_equal(command_handle(&command), 0);
}

int main(int argc, char *argv[])
{
	struct CMUnitTest tests[] = {
		cmocka_unit_test(test_add_command_no_param),
		cmocka_unit_test(test_add_command_cannot_create_task),
		cmocka_unit_test(test_add_command_cannot_save_task),
		cmocka_unit_test(test_add_command_success),
		cmocka_unit_test(test_add_command_success_multi_word),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
