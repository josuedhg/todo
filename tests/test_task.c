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
	assert_null(task2);
}

void test_negative_create_task_name_long(void **state)
{
	const char long_string[] = "This is a project with a very very very very"
		"that could be used as project name or any other kind of name"
		"in the project this is just with test purpuses but all validation"
		"is needed";
	struct task *task = create_task(long_string, "project", TASK_PRIORITY_LOW);
	assert_null(task);

	struct task *task2 = create_task("name", long_string, TASK_PRIORITY_LOW);
	assert_null(task2);
}

void test_create_task(void **state)
{
	struct task *task = create_task("name", "pname", TASK_PRIORITY_HIGH);
	assert_non_null(task);
	assert_string_equal(task->name, "name");
	assert_string_equal(task->project_name, "pname");
	assert_int_equal(task->priority, TASK_PRIORITY_HIGH);
	assert_non_null(task->ops);
	assert_non_null(task->ops->set_completed);
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
	assert_non_null(task->ops);
	assert_non_null(task->ops->set_completed);
}

void test_negative_set_completed_null(void **state)
{
	struct task task = {.ops = NULL};
	struct task_ops task_ops = {.set_completed = NULL};
	struct task task2 = {.ops = &task_ops};
	expect_assert_failure(task_set_completed(NULL));
	expect_assert_failure(task_set_completed(&task));
	expect_assert_failure(task_set_completed(&task2));
}

void test_task_set_completed(void **state)
{
	time_t before = time(NULL);
	struct task *task = create_task("name", "pname", TASK_PRIORITY_LOW);
	task_set_completed(task);
	assert_int_equal(task->status, TASK_STATUS_COMPLETED);
	assert_true(before <= task->completion_date);
}

int main(int argc, char *argv[])
{
	struct CMUnitTest tests[] = {
		cmocka_unit_test(test_negative_create_task_name_null),
		cmocka_unit_test(test_negative_create_task_name_long),
		cmocka_unit_test(test_create_task),
		cmocka_unit_test(test_create_new_task),
		cmocka_unit_test(test_negative_set_completed_null),
		cmocka_unit_test(test_task_set_completed),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
