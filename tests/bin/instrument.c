#define _GNU_SOURCE
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/queue.h>

#include "todo.h"

static bool is_instrumented = false;

void instrument_output(bool instrument)
{
	is_instrumented = instrument;
}

void mock_log_function(const char *fmt, ...)
{
	va_list args;
	char *report_string = NULL;

	va_start(args, fmt);
	vasprintf(&report_string, fmt, args);
	check_expected(report_string);

	free(report_string);
	va_end(args);
}

extern int __real_vfprintf(FILE *stream, const char *fmt, va_list args);
int __wrap_vfprintf(FILE *stream, const char *fmt, va_list args)
{
	if (!is_instrumented)
		return __real_vfprintf(stream, fmt, args);
	char *report_string = NULL;
	int ret = 0;

	ret = vasprintf(&report_string, fmt, args);
	check_expected(stream);
	check_expected(report_string);
	free(report_string);
	return ret;
}

// libtodo instrument functions
struct task *__wrap_create_new_task(const char *desc, const char *project, int priority)
{
	struct task *task = mock_ptr_type(struct task *);
	if (task == NULL)
		return NULL;
	check_expected(desc);
	check_expected(project);
	check_expected(priority);
	return task;
}

struct todo *__wrap_create_todotxt(const char *pathname)
{
	return mock_ptr_type(struct todo *);
}

void __wrap_destroy_todotxt(struct todo **todo)
{
	return;
}

void mock_todo_clean_tasks(struct todo *todo)
{
	return;
}

struct task *mock_todo_get_task(struct todo *todo, int id)
{
	return mock_ptr_type(struct task *);
}

int mock_todo_edit_task(struct todo *todo, struct task *task)
{
	return mock_type(int);
}

struct task *mock_todo_remove_task(struct todo *todo, int id)
{
	return mock_ptr_type(struct task *);
}

int mock_todo_add_task(struct todo *todo, struct task *task)
{
	return mock_type(int);
}

struct todo_driver mock_todo_driver = {
	.edit_task = mock_todo_edit_task,
	.add_task = mock_todo_add_task,
	.get_task = mock_todo_get_task,
	.remove_task = mock_todo_remove_task,
};
