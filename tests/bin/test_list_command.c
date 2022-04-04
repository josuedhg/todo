#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "todo.h"
#include "list_command.h"

// TODO: Check stdout and stderr for correct output

struct todo todo = {
	.task_counter = 0,
};

static void test_list_command_cannot_access_file(void **state)
{
	(void)state; /* unused */
	will_return(__wrap_create_todotxt, NULL);
	int ret = list_command.command_handle(0, NULL);
	assert_int_equal(ret, -1);
}

static void test_list_command_cannot_read_file(void **state)
{
	(void)state; /* unused */
	will_return(__wrap_create_todotxt, &todo);
	will_return(__wrap_todo_load_tasks, -1);
	int ret = list_command.command_handle(0, NULL);
	assert_int_equal(ret, -1);
}

static void test_list_command_no_tasks(void **state)
{
	(void)state; /* unused */
	will_return(__wrap_create_todotxt, &todo);
	will_return(__wrap_todo_load_tasks, 0);
	int ret = list_command.command_handle(0, NULL);
	assert_int_equal(ret, 0);
}

static void test_list_command_one_task(void **state)
{
	(void)state; /* unused */
	struct task *task = create_task("name", "project", TASK_PRIORITY_LOW);

	// setup todo
	todo.task_list[0] = task;
	todo.task_counter = 1;

	// prepare returns
	will_return(__wrap_create_todotxt, &todo);
	will_return(__wrap_todo_load_tasks, 0);

	// run command
	int ret = list_command.command_handle(0, NULL);

	// check results
	assert_int_equal(ret, 0);

	// cleanup
	todo.task_list[0] = NULL;
	todo.task_counter = 0;
	destroy_task(&task);
}

static void test_list_command_multiple_tasks(void **state)
{
	(void)state; /* unused */
	struct task *task1 = create_task("name1", "project1", TASK_PRIORITY_LOW);
	struct task *task2 = create_task("name2", "project2", TASK_PRIORITY_LOW);

	// setup todo
	todo.task_list[0] = task1;
	todo.task_list[1] = task2;
	todo.task_counter = 2;

	// prepare returns
	will_return(__wrap_create_todotxt, &todo);
	will_return(__wrap_todo_load_tasks, 0);

	// run command
	int ret = list_command.command_handle(0, NULL);

	// check results
	assert_int_equal(ret, 0);

	// cleanup
	todo.task_list[0] = NULL;
	todo.task_list[1] = NULL;
	todo.task_counter = 0;
	destroy_task(&task1);
	destroy_task(&task2);
}


int main(int argc, char *argv[])
{
	struct CMUnitTest tests[] = {
		cmocka_unit_test(test_list_command_cannot_access_file),
		cmocka_unit_test(test_list_command_cannot_read_file),
		cmocka_unit_test(test_list_command_no_tasks),
		cmocka_unit_test(test_list_command_one_task),
		cmocka_unit_test(test_list_command_multiple_tasks),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
