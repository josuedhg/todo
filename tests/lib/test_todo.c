#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "todo.h"

void test_todo_init(void **state)
{
	struct todo todo;
	todo_init(&todo);
	assert_int_equal(todo.task_counter, 0);
	assert_non_null(todo.task_list);
	assert_non_null(todo.ops);
	assert_non_null(todo.ops->load_tasks);
	assert_non_null(todo.ops->clean_tasks);
	assert_non_null(todo.ops->save_tasks);
	assert_non_null(todo.ops->add_task);
	assert_non_null(todo.ops->remove_task);
}

void test_negative_todo_load_tasks_null(void **state)
{
	int res = todo_load_tasks(NULL);
	assert_int_equal(res, -1);
}

void test_todo_load_tasks(void **state)
{
	struct todo todo;
	todo_init(&todo);
	int res = todo_load_tasks(&todo);
	assert_int_equal(res, 0);
}

void test_negative_todo_save_tasks_null(void **state)
{
	int res = todo_save_tasks(NULL);
	assert_int_equal(res, -1);
}

void test_todo_save_tasks(void **state)
{
	struct todo todo;
	todo_init(&todo);
	int res = todo_save_tasks(&todo);
	assert_int_equal(res, 0);
}

void test_negative_todo_add_task_null(void **state)
{
	struct todo todo;
	todo_init(&todo);
	struct task *task = create_new_task("name", "project", TASK_PRIORITY_LOW);
	expect_assert_failure(todo_add_task(NULL, task));
	expect_assert_failure(todo_add_task(&todo, NULL));
	expect_assert_failure(todo_add_task(NULL, NULL));
	destroy_task(&task);
}

void test_negative_todo_add_task_full(void **state)
{
	struct todo todo;
	todo_init(&todo);
	for (int i = 0; i < TODO_TASK_LIST_LENGTH; i++) {
		struct task *task = create_task("name", "project", TASK_PRIORITY_LOW);
		todo_add_task(&todo, task);
	}
	struct task *task = create_task("last_task", "project", TASK_PRIORITY_LOW);
	expect_assert_failure(todo_add_task(&todo, task));
	destroy_task(&task);
	todo_clean_tasks(&todo);
}

void test_todo_add_task_empty(void **state)
{
	struct todo todo;
	todo_init(&todo);
	struct task *task = create_new_task("name", "project", TASK_PRIORITY_LOW);
	todo_add_task(&todo, task);
	assert_memory_equal(task, todo.task_list[0], sizeof(struct task));
	assert_int_equal(todo.task_counter, 1);
	todo_clean_tasks(&todo);
}

void test_negative_todo_remove_task_null(void **state)
{
	struct todo todo;
	todo_init(&todo);
	struct task *task = create_task("name", "project", TASK_PRIORITY_LOW);
	expect_assert_failure(todo_remove_task(NULL, task));
	expect_assert_failure(todo_remove_task(&todo, NULL));
	expect_assert_failure(todo_remove_task(NULL, NULL));
	destroy_task(&task);
}

void test_negative_todo_remove_task_empty(void **state)
{
	struct todo todo;
	todo_init(&todo);
	struct task *task = create_task("name", "project", TASK_PRIORITY_LOW);
	todo_remove_task(&todo, task);
	destroy_task(&task);
}

void test_negative_todo_remove_task_not_found(void **state)
{
	struct todo todo;
	todo_init(&todo);
	struct task *task = create_task("name", "project", TASK_PRIORITY_LOW);
	struct task *task2 = create_task("name", "project", TASK_PRIORITY_LOW);
	todo_add_task(&todo, task);
	todo_remove_task(&todo, task2);
	assert_int_equal(todo.task_counter, 1);
	assert_memory_equal(task, todo.task_list[0], sizeof(struct task));
	destroy_task(&task2);
	todo_clean_tasks(&todo);
}

void test_todo_remove_task(void **state)
{
	struct todo todo;
	todo_init(&todo);
	struct task *task = create_task("name", "project", TASK_PRIORITY_LOW);
	struct task *task2 = create_task("name", "project", TASK_PRIORITY_LOW);
	todo_add_task(&todo, task);
	todo_add_task(&todo, task2);
	todo_remove_task(&todo, task);
	assert_int_equal(todo.task_counter, 1);
	assert_memory_equal(task2, todo.task_list[0], sizeof(struct task));
	destroy_task(&task);
	todo_clean_tasks(&todo);
}

void test_negative_todo_clean_tasks_null(void **state)
{
	expect_assert_failure(todo_clean_tasks(NULL));
}

void test_negative_todo_clean_tasks_empty(void **state)
{
	struct todo todo;
	todo_init(&todo);
	todo_clean_tasks(&todo);
}

void test_todo_clean_tasks(void **state)
{
	struct todo todo;
	todo_init(&todo);
	struct task *task = create_new_task("name", "project", TASK_PRIORITY_LOW);
	struct task *task2 = create_new_task("name", "project", TASK_PRIORITY_LOW);
	todo_add_task(&todo, task);
	todo_add_task(&todo, task2);
	todo_clean_tasks(&todo);
	assert_int_equal(todo.task_counter, 0);
	assert_null(todo.task_list[0]);
	assert_null(todo.task_list[1]);
}


int main(int argc, char *argv[])
{
	struct CMUnitTest tests[] = {
		cmocka_unit_test(test_todo_init),
		cmocka_unit_test(test_negative_todo_load_tasks_null),
		cmocka_unit_test(test_todo_load_tasks),
		cmocka_unit_test(test_negative_todo_save_tasks_null),
		cmocka_unit_test(test_todo_save_tasks),
		cmocka_unit_test(test_negative_todo_add_task_null),
		cmocka_unit_test(test_negative_todo_add_task_full),
		cmocka_unit_test(test_todo_add_task_empty),
		cmocka_unit_test(test_negative_todo_remove_task_null),
		cmocka_unit_test(test_negative_todo_remove_task_empty),
		cmocka_unit_test(test_negative_todo_remove_task_not_found),
		cmocka_unit_test(test_todo_remove_task),
		cmocka_unit_test(test_negative_todo_clean_tasks_null),
		cmocka_unit_test(test_negative_todo_clean_tasks_empty),
		cmocka_unit_test(test_todo_clean_tasks),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
