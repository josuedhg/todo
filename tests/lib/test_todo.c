#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "todo.h"

struct todo_driver todo_driver;

void test_todo_init(void **state)
{
	struct todo todo = {
		.driver = &todo_driver,
	};
	todo_init(&todo);
	assert_int_equal(todo.task_counter, 0);
	assert_non_null(todo.task_list);
	assert_non_null(todo.driver);
	assert_non_null(todo.driver->load_tasks);
	assert_non_null(todo.driver->save_tasks);
	assert_non_null(todo.driver->add_task);
	assert_non_null(todo.driver->remove_task);
}

void test_negative_todo_load_tasks_null(void **state)
{
	struct todo todo = {
		.driver = &todo_driver,
	};
	todo_init(&todo);
	int res = todo.driver->load_tasks(NULL);
	assert_int_equal(res, -1);
}

void test_todo_load_tasks(void **state)
{
	struct todo todo = {
		.driver = &todo_driver,
	};
	todo_init(&todo);
	int res = todo.driver->load_tasks(&todo);
	assert_int_equal(res, 0);
}

void test_negative_todo_save_tasks_null(void **state)
{
	struct todo todo = {
		.driver = &todo_driver,
	};
	todo_init(&todo);
	int res = todo.driver->save_tasks(NULL);
	assert_int_equal(res, -1);
}

void test_todo_save_tasks(void **state)
{
	struct todo todo = {
		.driver = &todo_driver,
	};
	todo_init(&todo);
	int res = todo.driver->save_tasks(&todo);
	assert_int_equal(res, 0);
}

void test_negative_todo_add_task_null(void **state)
{
	struct todo todo = {
		.driver = &todo_driver,
	};
	todo_init(&todo);
	struct task *task = create_new_task("name", "project", TASK_PRIORITY_LOW);
	assert_int_equal(todo.driver->add_task(NULL, task), -1);
	assert_int_equal(todo.driver->add_task(&todo, NULL), -1);
	assert_int_equal(todo.driver->add_task(NULL, NULL), -1);
	destroy_task(&task);
}

void test_negative_todo_add_task_full(void **state)
{
	struct todo todo = {
		.driver = &todo_driver,
	};
	todo_init(&todo);
	for (int i = 0; i < TODO_TASK_LIST_LENGTH; i++) {
		struct task *task = create_task("name", "project", TASK_PRIORITY_LOW);
		todo.driver->add_task(&todo, task);
	}
	struct task *task = create_task("last_task", "project", TASK_PRIORITY_LOW);
	assert_int_equal(todo.driver->add_task(&todo, task), -1);
	destroy_task(&task);
}

void test_todo_add_task_empty(void **state)
{
	struct todo todo = {
		.driver = &todo_driver,
	};
	todo_init(&todo);
	struct task *task = create_new_task("name", "project", TASK_PRIORITY_LOW);
	assert_int_equal(todo.driver->add_task(&todo, task), 0);
	assert_memory_equal(task, todo.task_list[0], sizeof(struct task));
	assert_int_equal(todo.task_counter, 1);
}

void test_negative_todo_remove_task_null(void **state)
{
	struct todo todo = {
		.driver = &todo_driver,
	};
	todo_init(&todo);
	struct task *task = create_task("name", "project", TASK_PRIORITY_LOW);
	assert_int_equal(todo.driver->remove_task(NULL, task), -1);
	assert_int_equal(todo.driver->remove_task(&todo, NULL), -1);
	assert_int_equal(todo.driver->remove_task(NULL, NULL), -1);
	destroy_task(&task);
}

void test_negative_todo_remove_task_empty(void **state)
{
	struct todo todo = {
		.driver = &todo_driver,
	};
	todo_init(&todo);
	struct task *task = create_task("name", "project", TASK_PRIORITY_LOW);
	todo.driver->remove_task(&todo, task);
	destroy_task(&task);
}

void test_negative_todo_remove_task_not_found(void **state)
{
	struct todo todo = {
		.driver = &todo_driver,
	};
	todo_init(&todo);
	struct task *task = create_task("name", "project", TASK_PRIORITY_LOW);
	struct task *task2 = create_task("name", "project", TASK_PRIORITY_LOW);
	todo.driver->add_task(&todo, task);
	todo.driver->remove_task(&todo, task2);
	assert_int_equal(todo.task_counter, 1);
	assert_memory_equal(task, todo.task_list[0], sizeof(struct task));
	destroy_task(&task2);
}

void test_todo_remove_task(void **state)
{
	struct todo todo = {
		.driver = &todo_driver,
	};
	todo_init(&todo);
	struct task *task = create_task("name", "project", TASK_PRIORITY_LOW);
	struct task *task2 = create_task("name", "project", TASK_PRIORITY_LOW);
	todo.driver->add_task(&todo, task);
	todo.driver->add_task(&todo, task2);
	todo.driver->remove_task(&todo, task);
	assert_int_equal(todo.task_counter, 1);
	assert_memory_equal(task2, todo.task_list[0], sizeof(struct task));
	destroy_task(&task);
}

void test_negative_todo_clean_tasks_null(void **state)
{
	expect_assert_failure(todo_clean_tasks(NULL));
}

void test_negative_todo_clean_tasks_empty(void **state)
{
	struct todo todo = {
		.driver = &todo_driver,
	};
	todo_init(&todo);
	todo_clean_tasks(&todo);
}

void test_todo_clean_tasks(void **state)
{
	struct todo todo = {
		.driver = &todo_driver,
	};
	todo_init(&todo);
	struct task *task = create_new_task("name", "project", TASK_PRIORITY_LOW);
	struct task *task2 = create_new_task("name", "project", TASK_PRIORITY_LOW);
	todo.driver->add_task(&todo, task);
	todo.driver->add_task(&todo, task2);
	todo_clean_tasks(&todo);
	assert_int_equal(todo.task_counter, 0);
	assert_null(todo.task_list[0]);
	assert_null(todo.task_list[1]);
}

void test_negative_todo_get_task_null(void **state)
{
	struct todo todo = {
		.driver = &todo_driver,
	};
	expect_assert_failure(todo.driver->get_task(NULL, 0));
}

void test_negative_todo_get_task_empty(void **state)
{
	struct todo todo = {
		.driver = &todo_driver,
	};
	todo_init(&todo);
	assert_null(todo.driver->get_task(&todo, 1));
}

void test_negative_todo_get_task_not_found(void **state)
{
	struct todo todo = {
		.driver = &todo_driver,
	};
	todo_init(&todo);
	struct task *task = create_new_task("name", "project", TASK_PRIORITY_LOW);
	task->id = 1;
	struct task *task2 = create_new_task("name", "project", TASK_PRIORITY_LOW);
	task2->id = 2;
	todo.driver->add_task(&todo, task);
	todo.driver->add_task(&todo, task2);
	assert_null(todo.driver->get_task(&todo, 4));
}

void test_negative_todo_get_task_found(void **state)
{
	struct todo todo = {
		.driver = &todo_driver,
	};
	todo_init(&todo);
	struct task *task = create_new_task("name", "project", TASK_PRIORITY_LOW);
	task->id = 1;
	struct task *task2 = create_new_task("name", "project", TASK_PRIORITY_LOW);
	task2->id = 2;
	todo.driver->add_task(&todo, task);
	todo.driver->add_task(&todo, task2);
	assert_non_null(todo.driver->get_task(&todo, 2));
}

bool iterator_callback(struct task *task, void *data)
{
	check_expected(task);
	check_expected(data);
	return mock_type(bool);
}

void test_negative_todo_iterator_next_null_param(void **state)
{
	expect_assert_failure(todo_iterator_next(NULL));
}

void test_todo_iterator_next_no_filter_no_tasks(void **state)
{
	struct todo todo = {
		.driver = &todo_driver,
	};
	todo_init(&todo);
	struct todo_iterator iterator = { .todo = &todo, .filter = NULL, .data = NULL, .index = 0 };
	struct task *task = todo_iterator_next(&iterator);
	assert_null(task);
}

void test_todo_iterator_next_no_filter_with_tasks(void **state)
{
	struct todo todo = {
		.driver = &todo_driver,
	};
	todo_init(&todo);
	todo.driver->add_task(&todo, create_new_task("my task", "myproject", TASK_PRIORITY_HIGH));
	struct todo_iterator iterator = { .todo = &todo, .filter = NULL, .data = NULL, .index = 0 };
	struct task *task = todo_iterator_next(&iterator);
	assert_non_null(task);
	task = todo_iterator_next(&iterator);
	assert_null(task);
}

void test_todo_iterator_next_filter_no_tasks(void **state)
{
	struct todo todo = {
		.driver = &todo_driver,
	};
	todo_init(&todo);
	struct todo_iterator iterator = {.todo = &todo, .filter = iterator_callback, .data = NULL, .index = 0};
	struct task *task = todo_iterator_next(&iterator);
	assert_null(task);
}

void test_todo_iterator_next_filter_with_tasks(void **state)
{
	struct todo todo = {
		.driver = &todo_driver,
	};
	todo_init(&todo);
	int data = 0xDEADBEEF;
	struct task *created_task = create_new_task("my task", "myproject", TASK_PRIORITY_HIGH);
	todo.driver->add_task(&todo, created_task);
	struct todo_iterator iterator = { .todo = &todo, .filter = iterator_callback, .data = &data, .index = 0 };

	expect_value(iterator_callback, task, created_task);
	expect_value(iterator_callback, data, &data);
	will_return(iterator_callback, true);
	struct task *task = todo_iterator_next(&iterator);
	assert_non_null(task);
	task = todo_iterator_next(&iterator);
	assert_null(task);
}

void test_todo_iterator_next_filter_with_tasks_and_filter_false(void **state)
{
	struct todo todo = {
		.driver = &todo_driver,
	};
	todo_init(&todo);
	int data = 0xDEADBEEF;
	struct task *created_task = create_new_task("my task", "myproject", TASK_PRIORITY_HIGH);
	todo.driver->add_task(&todo, created_task);
	struct todo_iterator iterator = { .todo = &todo, .filter = iterator_callback, .data = &data, .index = 0 };

	expect_value(iterator_callback, task, created_task);
	expect_value(iterator_callback, data, &data);
	will_return(iterator_callback, false);
	struct task *task = todo_iterator_next(&iterator);
	assert_null(task);
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
		cmocka_unit_test(test_negative_todo_get_task_null),
		cmocka_unit_test(test_negative_todo_get_task_empty),
		cmocka_unit_test(test_negative_todo_get_task_not_found),
		cmocka_unit_test(test_negative_todo_get_task_found),
		cmocka_unit_test(test_negative_todo_iterator_next_null_param),
		cmocka_unit_test(test_todo_iterator_next_no_filter_no_tasks),
		cmocka_unit_test(test_todo_iterator_next_no_filter_with_tasks),
		cmocka_unit_test(test_todo_iterator_next_filter_no_tasks),
		cmocka_unit_test(test_todo_iterator_next_filter_with_tasks),
		cmocka_unit_test(test_todo_iterator_next_filter_with_tasks_and_filter_false),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
