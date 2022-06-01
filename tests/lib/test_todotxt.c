#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "task.h"
#include "todotxt.h"
#include "todotxt_i.h"

void test_negative_todotxt_get_time_from_string_null(void **status)
{
	time_t time = 0;
	expect_assert_failure(todotxt_get_time_from_string(NULL, &time));
	expect_assert_failure(todotxt_get_time_from_string("2001-02-12", NULL));
	expect_assert_failure(todotxt_get_time_from_string(NULL, NULL));
}

void test_negative_todotxt_get_time_from_string_bad_format(void **state)
{
	time_t time = 0;
	assert_false(todotxt_get_time_from_string("bad format", &time));
	assert_false(todotxt_get_time_from_string("2001/02/12", &time));
	assert_false(todotxt_get_time_from_string("2001-14-01", &time));
	assert_false(todotxt_get_time_from_string("2001-11-34", &time));
}

void test_todotxt_get_time_from_string(void **state)
{
	time_t time = 0;
	assert_true(todotxt_get_time_from_string("2001-02-22", &time));
	assert_true(time > 0);
}

void test_negative_todotxt_get_status_null(void **state)
{
	time_t time = 0;
	expect_assert_failure(todotxt_get_status(NULL, &time));
	expect_assert_failure(todotxt_get_status("x 2001-12-02", NULL));
	expect_assert_failure(todotxt_get_status(NULL, NULL));
}

void test_negative_todotxt_get_status_short(void **status)
{
	time_t time = 0;
	assert_false(todotxt_get_status("x 2001", &time));
}

void test_negative_todotxt_get_status_bad_format(void **status)
{
	time_t time = 0;
	assert_false(todotxt_get_status("bad format status", &time));
	assert_false(todotxt_get_status("xbad format status", &time));
	assert_false(todotxt_get_status("x bad format status", &time));
}

void test_todotxt_get_status(void **status)
{
	time_t time = 0;
	assert_true(todotxt_get_status("x 2002-05-12", &time));
	assert_true(time > 0);
}

void test_todotxt_get_status_with_priority(void **state)
{
	time_t time = 0;
	assert_true(todotxt_get_status("x (A) 2002-05-12", &time));
	assert_true(time > 0);
}

void test_negative_todotxt_get_project_name_from_desc(void **state)
{
	expect_assert_failure(todotxt_get_project_name_from_desc(NULL));
}

void test_negative_todotxt_get_project_name_from_desc_bad_format(void **state)
{
	char *project_name = todotxt_get_project_name_from_desc("something+myproject something");
	assert_null(project_name);

	project_name = todotxt_get_project_name_from_desc("something + myproject something");
	assert_null(project_name);

	project_name = todotxt_get_project_name_from_desc("something+ something");
	assert_null(project_name);

	project_name = todotxt_get_project_name_from_desc("something something +");
	assert_null(project_name);

	project_name = todotxt_get_project_name_from_desc("something something");
	assert_null(project_name);
}

void test_todotxt_get_project_name_from_desc(void **state)
{
	char *project_name = todotxt_get_project_name_from_desc("something +myproject something");
	assert_string_equal(project_name, "myproject");

	char *project_name2 = todotxt_get_project_name_from_desc("+myproject something something");
	assert_string_equal(project_name2, "myproject");

	char *project_name3 = todotxt_get_project_name_from_desc("something something +myproject");
	assert_string_equal(project_name3, "myproject");

	char *project_name4 = todotxt_get_project_name_from_desc("+myproject something something +myproject2");
	assert_string_equal(project_name4, "myproject");
}

void test_negative_todotxt_get_priority_null(void **state)
{
	expect_assert_failure(todotxt_get_priority(NULL));
}

void test_negative_todotxt_get_priority_bad_format(void **state)
{
	assert_int_equal(todotxt_get_priority(""), NO_PRIORITY_FORMAT);
	assert_int_equal(todotxt_get_priority("something"), NO_PRIORITY_FORMAT);
	assert_int_equal(todotxt_get_priority("(something)"), NO_PRIORITY_FORMAT);
	assert_int_equal(todotxt_get_priority("(a)"), NO_PRIORITY_FORMAT);
}

void test_todotxt_get_priority(void **state)
{
	assert_int_equal(todotxt_get_priority("(A) something"), TASK_PRIORITY_HIGH);
	assert_int_equal(todotxt_get_priority("(B) something"), TASK_PRIORITY_MEDIUM);
	assert_int_equal(todotxt_get_priority("(C) something"), TASK_PRIORITY_LOW);
	assert_int_equal(todotxt_get_priority("(D) something"), 3);
}

void test_negative_todotxt_get_duedate_from_desc_null(void **state)
{
	time_t duedate = 0;
	expect_assert_failure(todotxt_get_duedate_from_desc(NULL, &duedate));
	expect_assert_failure(todotxt_get_duedate_from_desc("due:2002-05-12", NULL));
	expect_assert_failure(todotxt_get_duedate_from_desc(NULL, NULL));
}

void test_negative_todotxt_get_duedate_from_desc_bad_format(void **state)
{
	time_t duedate = 0;
	assert_false(todotxt_get_duedate_from_desc("", &duedate));
	assert_false(todotxt_get_duedate_from_desc("due 2002-05-12", &duedate));
	assert_false(todotxt_get_duedate_from_desc("due:2002-55-12", &duedate));
	assert_false(todotxt_get_duedate_from_desc("due:20A2-55-12", &duedate));
	assert_false(todotxt_get_duedate_from_desc("something", &duedate));
	assert_false(todotxt_get_duedate_from_desc("somethingdue:2002-05-12", &duedate));
	assert_false(todotxt_get_duedate_from_desc("something due2002-05-12", &duedate));
}

void test_todotxt_get_duedate_from_desc(void **state)
{
	time_t duedate = 0;
	assert_true(todotxt_get_duedate_from_desc("due:2002-05-12 something", &duedate));
	assert_true(duedate > 0);
}

void test_negative_create_task_from_todotxt_line_null(void **state)
{
	struct task *task = create_task_from_todotxt(NULL);
	assert_null(task);
}

void test_create_task_from_todottxt_line(void **state)
{
	struct task *task = create_task_from_todotxt("new task");
	assert_non_null(task);
	assert_string_equal(task->name, "new task");
	assert_int_equal(task->status, TASK_STATUS_OPEN);
	assert_int_equal(task->priority, TASK_PRIORITY_LOW);

	struct task *task1 = create_task_from_todotxt("new task other due:2021-12-30");
	assert_non_null(task1);
	assert_string_equal(task1->name, "new task other due:2021-12-30");
	assert_true(task1->due_date > 0);
	assert_int_equal(task1->status, TASK_STATUS_OPEN);
	assert_int_equal(task1->priority, TASK_PRIORITY_LOW);

	struct task *task2 = create_task_from_todotxt("(A) task with priority");
	assert_non_null(task2);
	assert_string_equal(task2->name, "task with priority");
	assert_int_equal(task2->priority, TASK_PRIORITY_HIGH);
	assert_int_equal(task2->status, TASK_STATUS_OPEN);

	struct task *task3 = create_task_from_todotxt("x 2011-03-03 do something");
	assert_non_null(task3);
	assert_string_equal(task3->name, "do something");
	assert_true(task3->completion_date > 0);
	assert_int_equal(task3->status, TASK_STATUS_COMPLETED);
	assert_int_equal(task3->priority, TASK_PRIORITY_LOW);

	struct task *task4 = create_task_from_todotxt("task of +project");
	assert_non_null(task4);
	assert_string_equal(task4->name, "task of +project");
	assert_string_equal(task4->project_name, "project");
	assert_int_equal(task1->status, TASK_STATUS_OPEN);
	assert_int_equal(task1->priority, TASK_PRIORITY_LOW);

	struct task *task5 = create_task_from_todotxt("x (B) 2011-03-03 2011-03-03 task of +myproject due:2012-01-01");
	assert_non_null(task5);
	assert_int_equal(task5->status, TASK_STATUS_COMPLETED);
	assert_int_equal(task5->priority, TASK_PRIORITY_MEDIUM);
	assert_true(task5->completion_date > 0);
	assert_true(task5->creation_date > 0);
	assert_string_equal(task5->name, "task of +myproject due:2012-01-01");
	assert_string_equal(task5->project_name, "myproject");

	destroy_task(&task);
	destroy_task(&task1);
	destroy_task(&task2);
	destroy_task(&task3);
	destroy_task(&task4);
	destroy_task(&task5);
}

void test_negative_create_todotxt_line_from_task_null(void **state)
{
	int todotxtline_len = 0;
	char *todotxtline_buffer = NULL;
	struct task *task = create_task("name", "pname", TASK_PRIORITY_HIGH);

	todotxtline_len = create_todotxt_line_from_task(NULL, NULL);
	assert_int_equal(todotxtline_len, -1);

	todotxtline_len = create_todotxt_line_from_task(NULL, &todotxtline_buffer);
	assert_int_equal(todotxtline_len, -1);

	todotxtline_len = create_todotxt_line_from_task(task, NULL);
	assert_int_equal(todotxtline_len, -1);
	destroy_task(&task);
}

int func(struct task *task, char **buffer)
{
	if (task == NULL || buffer == NULL)
		return -1;
	int len = 32;
	*buffer = calloc(len, sizeof(char));
	return len;
}

#define FIRST_DAY_UNIX_TIME (time_t)86400
#define DATE_FORMAT "%Y-%m-%d"
#define DATE_LENGHT 11

void test_create_todotxt_line_from_task(void **state)
{
	int todotxtline_len = 0;
	char *todotxtline_buffer = NULL;
	struct task *task = create_new_task("my task", "myproject", TASK_PRIORITY_HIGH);
	task->creation_date = FIRST_DAY_UNIX_TIME;

	todotxtline_len = create_todotxt_line_from_task(task, &todotxtline_buffer);
	assert_int_not_equal(todotxtline_len, -1);

	char creation_date[DATE_LENGHT] = { 0 };
	strftime(creation_date, DATE_LENGHT, DATE_FORMAT, localtime(&task->creation_date));

#define EXPECTED_LINE_SIZE 34
	char expected_todotxtline[EXPECTED_LINE_SIZE] = { 0 };
	sprintf(expected_todotxtline, "(A) %s my task +myproject", creation_date);

	assert_string_equal(todotxtline_buffer, expected_todotxtline);

	destroy_task(&task);
	free(todotxtline_buffer);
}

void test_create_todotxt_line_from_task_completed(void **state)
{
	int todotxtline_len = 0;
	char *todotxtline_buffer = NULL;
	struct task *task = create_new_task("my task", "myproject", TASK_PRIORITY_HIGH);

	task_set_completed(task);

	// set dates as 0 to have control in the date
	task->creation_date = FIRST_DAY_UNIX_TIME;
	task->completion_date = FIRST_DAY_UNIX_TIME;

	todotxtline_len = create_todotxt_line_from_task(task, &todotxtline_buffer);
	assert_int_not_equal(todotxtline_len, -1);

	char creation_date[DATE_LENGHT] = { 0 };
	strftime(creation_date, DATE_LENGHT, DATE_FORMAT, localtime(&task->creation_date));

	char completion_date[DATE_LENGHT] = { 0 };
	strftime(completion_date, DATE_LENGHT, DATE_FORMAT, localtime(&task->completion_date));

#define EXPECTED_COMPLETED_LINE_SIZE 47
	char expected_todotxtline[EXPECTED_COMPLETED_LINE_SIZE] = { 0 };
	sprintf(expected_todotxtline, "x (A) %s %s my task +myproject", creation_date, completion_date);

	assert_string_equal(todotxtline_buffer, expected_todotxtline);

	destroy_task(&task);
	free(todotxtline_buffer);
}

void test_create_todotxt_line_from_task_completed_with_date(void **state)
{
	int todotxtline_len = 0;
	char *todotxtline_buffer = NULL;
	struct task *task = create_new_task("my task", "myproject", TASK_PRIORITY_HIGH);
	task_set_completed(task);

	// set dates as 0 to have control in the date
	task->creation_date = FIRST_DAY_UNIX_TIME;
	task->completion_date = FIRST_DAY_UNIX_TIME;

	task->due_date = FIRST_DAY_UNIX_TIME + 86400;

	todotxtline_len = create_todotxt_line_from_task(task, &todotxtline_buffer);
	assert_int_not_equal(todotxtline_len, -1);

	char creation_date[DATE_LENGHT] = { 0 };
	strftime(creation_date, DATE_LENGHT, DATE_FORMAT, localtime(&task->creation_date));

	char completion_date[DATE_LENGHT] = { 0 };
	strftime(completion_date, DATE_LENGHT, DATE_FORMAT, localtime(&task->completion_date));

	char due_date[DATE_LENGHT] = { 0 };
	strftime(due_date, DATE_LENGHT, DATE_FORMAT, localtime(&task->due_date));

#define EXPECTED_DUE_DATE_LINE_SIZE 62
	char expected_todotxtline[EXPECTED_DUE_DATE_LINE_SIZE] = { 0 };
	sprintf(expected_todotxtline, "x (A) %s %s my task +myproject due:%s", creation_date, completion_date, due_date);

	assert_string_equal(todotxtline_buffer, expected_todotxtline);

	destroy_task(&task);
	free(todotxtline_buffer);
}

int setup(void **state)
{
	will_return(__wrap_access, 0);
	struct todo *todo = create_todotxt("filename");
	*state = todo;
	errno = 0;
	return 0;
}

extern void wrap_fwrite();
extern void reset_fwrite();
int teardown(void **state)
{
	struct todo *todo = (struct todo *)*state;
	destroy_todotxt(&todo);
	reset_fwrite();
	return 0;
}

void test_negative_create_todotxt_null(void **state)
{
	struct todo *todo = create_todotxt(NULL);
	assert_null(todo);
}

void test_negative_create_todotxt_bad_file(void **state)
{
	will_return(__wrap_access, -1);
	struct todo *todo = create_todotxt("badfilename");
	assert_null(todo);
}

void test_create_todotxt(void **state)
{
	will_return(__wrap_access, 0);
	struct todo *todo = create_todotxt("filename");
	struct todotxt *todotxt = container_of(todo,
					       struct todotxt,
					       todo);
	assert_non_null(todo);
	assert_int_equal(todo->task_counter, 0);
	assert_non_null(todo->task_list);
	assert_non_null(todo->driver);
	assert_non_null(todo->driver->load_tasks);
	assert_non_null(todo->driver->clean_tasks);
	assert_non_null(todo->driver->save_tasks);
	assert_non_null(todo->driver->add_task);
	assert_non_null(todo->driver->remove_task);

	assert_string_equal(todotxt->filename, "filename");
	destroy_todotxt(&todo);
}

void test_negative_todotxt_load_tasks_cannot_open(void **state)
{
	struct todo *todo = (struct todo *)*state;
	will_return(__wrap_fopen, NULL);
	int res = todo->driver->load_tasks(todo);
	assert_int_equal(res, -1);
}

void test_negative_todo_load_tasks_bad_read(void **state)
{
	struct todo *todo = (struct todo *)*state;
	int file_p = 1;
	will_return(__wrap_fopen, &file_p);
	will_return(__wrap_getline, EINVAL);
	will_return(__wrap_getline, NULL);
	will_return(__wrap_getline, -1);
	int res = todo->driver->load_tasks(todo);
	assert_int_equal(res, -1);
}

void test_todo_load_tasks(void **state)
{
	struct todo *todo = (struct todo *)*state;
	int file_p = 1;
#define SAMPLE_TEXT_LEN 9
	char *line = calloc(1, sizeof(char) * SAMPLE_TEXT_LEN + 1);
	memcpy(line, "new task\n", SAMPLE_TEXT_LEN);
	will_return(__wrap_fopen, &file_p);
	will_return(__wrap_getline, 0);
	will_return(__wrap_getline, line);
	will_return(__wrap_getline, strlen(line));
	will_return(__wrap_getline, 0);
	will_return(__wrap_getline, NULL);
	will_return(__wrap_getline, -1);
	int res = todo->driver->load_tasks(todo);
	assert_int_equal(res, 0);
	assert_int_equal(todo->task_counter, 1);
	assert_string_equal(todo->task_list[0]->name, "new task");
	free(line);
}

void test_negative_todo_save_tasks_cannot_open(void **state)
{
	struct todo *todo = (struct todo *)*state;
	will_return(__wrap_fopen, NULL);
	int res = todo->driver->save_tasks(todo);
	assert_int_equal(res, -1);
}

void test_negative_todo_save_tasks_bad_write(void **state)
{
	wrap_fwrite();
	struct todo *todo = (struct todo *)*state;
	struct task *task = create_new_task("my task", "myproject", TASK_PRIORITY_HIGH);
	todo_add_task(todo, task);
	int file_p = 1;
	will_return(__wrap_fopen, &file_p);
	will_return(__wrap_fwrite, EINVAL);
	will_return(__wrap_fwrite, -1);
	int res = todo->driver->save_tasks(todo);
	assert_int_equal(res, -1);
}

void test_todo_save_tasks(void **state)
{
	wrap_fwrite();
	struct todo *todo = (struct todo *)*state;
	struct task *task = create_new_task("my task", "myproject", TASK_PRIORITY_HIGH);
	task->creation_date = FIRST_DAY_UNIX_TIME;
	todo_add_task(todo, task);

	char creation_date[DATE_LENGHT] = { 0 };
	strftime(creation_date, DATE_LENGHT, DATE_FORMAT, localtime(&task->creation_date));

#define EXPECTED_LINE_SIZE_IN_FILE 33
	char expected_todotxtline[EXPECTED_LINE_SIZE_IN_FILE] = { 0 };
	sprintf(expected_todotxtline, "(A) %s my task +myproject", creation_date);

	int file_p = 1;
	will_return(__wrap_fopen, &file_p);
	will_return(__wrap_fwrite, 0);
	will_return(__wrap_fwrite, 1);

	expect_value(__wrap_fwrite, size, EXPECTED_LINE_SIZE_IN_FILE);
	expect_memory(__wrap_fwrite, ptr, expected_todotxtline, EXPECTED_LINE_SIZE_IN_FILE);

	int res = todo->driver->save_tasks(todo);
	assert_int_equal(res, 0);
}

void test_todo_save_tasks_multiline(void **state)
{
	wrap_fwrite();
	struct todo *todo = (struct todo *)*state;

	struct task *task = create_new_task("my task", "myproject", TASK_PRIORITY_HIGH);
	task->creation_date = FIRST_DAY_UNIX_TIME;
	todo_add_task(todo, task);

	struct task *task2 = create_new_task("my tazk", "myproject", TASK_PRIORITY_HIGH);
	task2->creation_date = FIRST_DAY_UNIX_TIME;
	todo_add_task(todo, task2);

	char creation_date[DATE_LENGHT] = { 0 };
	strftime(creation_date, DATE_LENGHT, DATE_FORMAT, localtime(&task->creation_date));

	// task1
	char expected_todotxtline[EXPECTED_LINE_SIZE_IN_FILE + 1] = { 0 };
	sprintf(expected_todotxtline, "(A) %s my task +myproject\n", creation_date);

	int file_p = 1;
	will_return(__wrap_fopen, &file_p);
	will_return(__wrap_fwrite, 0);
	will_return(__wrap_fwrite, 1);

	expect_value(__wrap_fwrite, size, EXPECTED_LINE_SIZE_IN_FILE + 1);
	expect_memory(__wrap_fwrite, ptr, expected_todotxtline, EXPECTED_LINE_SIZE_IN_FILE + 1);

	// task2
	char expected_todotxtline2[EXPECTED_LINE_SIZE_IN_FILE] = { 0 };
	sprintf(expected_todotxtline2, "(A) %s my tazk +myproject", creation_date);

	will_return(__wrap_fwrite, 0);
	will_return(__wrap_fwrite, 1);

	expect_value(__wrap_fwrite, size, EXPECTED_LINE_SIZE_IN_FILE);
	expect_memory(__wrap_fwrite, ptr, expected_todotxtline2, EXPECTED_LINE_SIZE_IN_FILE);

	int res = todo->driver->save_tasks(todo);
	assert_int_equal(res, 0);
}

int main(int argc, char *argv[])
{
	struct CMUnitTest tests[] = {
		cmocka_unit_test(test_negative_todotxt_get_time_from_string_null),
		cmocka_unit_test(test_negative_todotxt_get_time_from_string_bad_format),
		cmocka_unit_test(test_todotxt_get_time_from_string),
		cmocka_unit_test(test_negative_todotxt_get_status_null),
		cmocka_unit_test(test_negative_todotxt_get_status_short),
		cmocka_unit_test(test_negative_todotxt_get_status_bad_format),
		cmocka_unit_test(test_todotxt_get_status),
		cmocka_unit_test(test_todotxt_get_status_with_priority),
		cmocka_unit_test(test_negative_todotxt_get_project_name_from_desc),
		cmocka_unit_test(test_negative_todotxt_get_project_name_from_desc_bad_format),
		cmocka_unit_test(test_todotxt_get_project_name_from_desc),
		cmocka_unit_test(test_negative_todotxt_get_priority_null),
		cmocka_unit_test(test_negative_todotxt_get_priority_bad_format),
		cmocka_unit_test(test_todotxt_get_priority),
		cmocka_unit_test(test_negative_todotxt_get_duedate_from_desc_null),
		cmocka_unit_test(test_negative_todotxt_get_duedate_from_desc_bad_format),
		cmocka_unit_test(test_todotxt_get_duedate_from_desc),
		cmocka_unit_test(test_negative_create_task_from_todotxt_line_null),
		cmocka_unit_test(test_create_task_from_todottxt_line),
		cmocka_unit_test(test_negative_create_todotxt_line_from_task_null),
		cmocka_unit_test(test_create_todotxt_line_from_task),
		cmocka_unit_test(test_create_todotxt_line_from_task_completed),
		cmocka_unit_test(test_create_todotxt_line_from_task_completed_with_date),
		cmocka_unit_test(test_negative_create_todotxt_null),
		cmocka_unit_test(test_negative_create_todotxt_bad_file),
		cmocka_unit_test(test_create_todotxt),
		cmocka_unit_test_setup_teardown(test_negative_todotxt_load_tasks_cannot_open, setup, teardown),
		cmocka_unit_test_setup_teardown(test_negative_todo_load_tasks_bad_read, setup, teardown),
		cmocka_unit_test_setup_teardown(test_todo_load_tasks, setup, teardown),
		cmocka_unit_test_setup_teardown(test_negative_todo_save_tasks_cannot_open, setup, teardown),
		cmocka_unit_test_setup_teardown(test_negative_todo_save_tasks_bad_write, setup, teardown),
		cmocka_unit_test_setup_teardown(test_todo_save_tasks, setup, teardown),
		cmocka_unit_test_setup_teardown(test_todo_save_tasks_multiline, setup, teardown),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
