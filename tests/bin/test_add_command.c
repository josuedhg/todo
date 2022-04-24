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

static void test_add_command_listed_in_help(void **state)
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
	assert_non_null(strstr(buffer, "add: Add a new task"));
	free(buffer);
}

static void test_add_command_no_param(void **state)
{
	(void)state; /* unused */
	char *buffer = NULL;
	size_t buffer_size = 0;

	instrument_stderr();
	assert_int_equal(add_command.command_handle(&todo, 0, NULL), -1);
	buffer_size = get_stderr_buffer(&buffer);
	deinstrument_stderr();

	assert_int_not_equal(buffer_size, 0);
	assert_non_null(strstr(buffer, "Usage: add <task description> \n"));
	free(buffer);
}

static void test_add_command_cannot_create_task(void **state)
{
	(void)state; /* unused */
	char *params[] = {"add", "task"};
	char *buffer = NULL;
	size_t buffer_size = 0;

	instrument_stderr();
	will_return(__wrap_create_new_task, NULL);
	assert_int_equal(add_command.command_handle(&todo, 2, params), -1);
	buffer_size = get_stderr_buffer(&buffer);
	deinstrument_stderr();

	assert_int_not_equal(buffer_size, 0);
	assert_non_null(strstr(buffer, "Error: Unable to create new task: task\n"));
	free(buffer);
}

extern struct task *__real_create_new_task(const char *, const char *, int);

static void test_add_command_cannot_save_task(void **state)
{
	(void)state; /* unused */
	char *params[] = {"add", "task"};
	struct task *task = __real_create_new_task("name", "project", TASK_PRIORITY_LOW);
	char *buffer = NULL;
	size_t buffer_size = 0;

	instrument_stderr();
	will_return(__wrap_create_new_task, task);
	expect_string(__wrap_create_new_task, desc, "task");
	expect_value(__wrap_create_new_task, project, NULL);
	expect_value(__wrap_create_new_task, priority, TASK_PRIORITY_LOW);
	will_return(__wrap_todo_save_tasks, -1);
	assert_int_equal(add_command.command_handle(&todo, 2, params), -1);
	buffer_size = get_stderr_buffer(&buffer);
	deinstrument_stderr();

	assert_int_not_equal(buffer_size, 0);
	assert_non_null(strstr(buffer, "Error: Unable to save task"));
	free(buffer);
	destroy_task(&task);
}

static void test_add_command_success(void **state)
{
	(void)state; /* unused */
	char *params[] = {"add", "task"};
	struct task *task = __real_create_new_task("name", "project", TASK_PRIORITY_LOW);
	char *buffer = NULL;
	size_t buffer_size = 0;

	instrument_stderr();
	will_return(__wrap_create_new_task, (struct task *)&task);
	expect_string(__wrap_create_new_task, desc, "task");
	expect_value(__wrap_create_new_task, project, NULL);
	expect_value(__wrap_create_new_task, priority, TASK_PRIORITY_LOW);
	will_return(__wrap_todo_save_tasks, 0);
	assert_int_equal(add_command.command_handle(&todo, 2, params), 0);
	buffer_size = get_stderr_buffer(&buffer);
	deinstrument_stderr();

	assert_int_equal(buffer_size, 0);
	free(buffer);
	destroy_task(&task);
}

static void test_add_command_success_multi_word(void **state)
{
	(void)state; /* unused */
	char *params[] = {"add", "task", "with", "spaces"};
	int fake_task = 1;
	char *buffer = NULL;
	size_t buffer_size = 0;

	instrument_stderr();
	will_return(__wrap_create_new_task, (struct task *)&fake_task);
	expect_string(__wrap_create_new_task, desc, "task with spaces");
	expect_value(__wrap_create_new_task, project, NULL);
	expect_value(__wrap_create_new_task, priority, TASK_PRIORITY_LOW);
	will_return(__wrap_todo_save_tasks, 0);
	assert_int_equal(add_command.command_handle(&todo, 4, params), 0);
	buffer_size = get_stderr_buffer(&buffer);
	deinstrument_stderr();

	assert_int_equal(buffer_size, 0);
	free(buffer);
}

int main(int argc, char *argv[])
{
	struct CMUnitTest tests[] = {
		cmocka_unit_test(test_add_command_listed_in_help),
		cmocka_unit_test(test_add_command_no_param),
		cmocka_unit_test(test_add_command_cannot_create_task),
		cmocka_unit_test(test_add_command_cannot_save_task),
		cmocka_unit_test(test_add_command_success),
		cmocka_unit_test(test_add_command_success_multi_word),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
