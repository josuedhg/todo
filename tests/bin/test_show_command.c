#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "todo.h"
#include "commands.h"

struct todo todo = {
	.task_counter = 0,
};

static void test_show_command_no_param(void **state)
{
	(void)state; /* unused */
	int ret = show_command.command_handle(0, NULL);
	assert_int_equal(ret, -1);
}

static void test_show_command_invalid_param(void **state)
{
	(void)state; /* unused */
	char *params[] = {"show", "invalid"};
	int ret = show_command.command_handle(2, params);
	assert_int_equal(ret, -1);
}

static void test_show_command_cannot_access_file(void **state)
{
	(void)state; /* unused */
	char *params[] = {"show", "1"};
	will_return(__wrap_create_todotxt, NULL);
	int ret = show_command.command_handle(2, params);
	assert_int_equal(ret, -1);
}

static void test_show_command_cannot_read_file(void **state)
{
	(void)state; /* unused */
	char *params[] = {"show", "1"};
	will_return(__wrap_create_todotxt, &todo);
	will_return(__wrap_todo_load_tasks, -1);
	int ret = show_command.command_handle(2, params);
	assert_int_equal(ret, -1);
}

static void test_show_command_task_not_found(void **state)
{
	(void)state; /* unused */
	char *params[] = {"show", "1"};
	will_return(__wrap_create_todotxt, &todo);
	will_return(__wrap_todo_load_tasks, 0);
	int ret = show_command.command_handle(2, params);
	assert_int_equal(ret, -1);
}

static void test_show_command_task_found(void **state)
{
	(void)state; /* unused */
	char *params[] = {"show", "1"};
	struct task *task = create_task("name", "project", TASK_PRIORITY_LOW);

	// setup todo
	todo.task_list[0] = task;
	todo.task_counter = 1;

	// prepare returns
	will_return(__wrap_create_todotxt, &todo);
	will_return(__wrap_todo_load_tasks, 0);

	// run command
	int ret = show_command.command_handle(2, params);

	// check results
	assert_int_equal(ret, 0);

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
		cmocka_unit_test(test_show_command_cannot_access_file),
		cmocka_unit_test(test_show_command_cannot_read_file),
		cmocka_unit_test(test_show_command_task_not_found),
		cmocka_unit_test(test_show_command_task_found),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
