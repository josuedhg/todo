#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "task.h"

void test_negative_create_task_name_null(void **state)
{
	struct task *task = create_task(NULL, "project", TASK_PRIORITY_LOW);
	assert_null(task);

	struct task *task2 = create_task("name", NULL, TASK_PRIORITY_LOW);
	assert_non_null(task2);
	destroy_task(&task2);
}

void test_create_task(void **state)
{
	struct task *task = create_task("name", "pname", TASK_PRIORITY_HIGH);
	assert_non_null(task);
	assert_string_equal(task->name, "name");
	assert_string_equal(task->project_name, "pname");
	assert_int_equal(task->priority, TASK_PRIORITY_HIGH);
	destroy_task(&task);
}

void test_negative_create_new_task_null(void **state)
{
	struct task *task = create_new_task(NULL, NULL, TASK_PRIORITY_LOW);
	assert_null(task);
}

void test_create_new_task(void **state)
{
	time_t before = time(NULL);
	struct task *task = create_new_task("name", "pname", TASK_PRIORITY_LOW);
	assert_non_null(task);
	assert_string_equal(task->name, "name");
	assert_string_equal(task->project_name, "pname");
	assert_int_equal(task->priority, TASK_PRIORITY_LOW);
	assert_true(before <= task->creation_date);
	destroy_task(&task);
}

void test_negative_set_completed_null(void **state)
{
	struct task task = {};
	expect_assert_failure(task_set_completed(NULL));
}

void test_task_set_completed(void **state)
{
	time_t before = time(NULL);
	struct task *task = create_task("name", "pname", TASK_PRIORITY_LOW);
	task_set_completed(task);
	assert_int_equal(task->status, TASK_STATUS_COMPLETED);
	assert_true(before <= task->completion_date);
	destroy_task(&task);
}

void test_negative_task_reopen_null(void **state)
{
	expect_assert_failure(task_reopen(NULL));
}

void test_task_reopen(void **state)
{
	struct task *task = create_task("name", "pname", TASK_PRIORITY_LOW);
	task_set_completed(task);
	task_reopen(task);
	assert_int_equal(task->status, TASK_STATUS_OPEN);
	assert_false(task->completion_date);
	destroy_task(&task);
}

void test_negative_destroy_task(void **state)
{
	destroy_task(NULL);
}

void test_destroy_task(void **state)
{
	struct task *task = create_task("name", "pname", TASK_PRIORITY_LOW);
	assert_non_null(task);
	destroy_task(&task);
	assert_null(task);
}

int main(int argc, char *argv[])
{
	struct CMUnitTest tests[] = {
		cmocka_unit_test(test_negative_create_task_name_null),
		cmocka_unit_test(test_create_task),
		cmocka_unit_test(test_create_new_task),
		cmocka_unit_test(test_negative_create_new_task_null),
		cmocka_unit_test(test_negative_set_completed_null),
		cmocka_unit_test(test_task_set_completed),
		cmocka_unit_test(test_negative_task_reopen_null),
		cmocka_unit_test(test_task_reopen),
		cmocka_unit_test(test_negative_destroy_task),
		cmocka_unit_test(test_destroy_task),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
