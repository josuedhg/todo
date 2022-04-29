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

struct entry {
	char *data;
	int data_len;
	TAILQ_ENTRY(entry) entries;
};

TAILQ_HEAD(entryhead, entry);

static bool instrument_output[3] = {false, false, false};
static struct entryhead entries[3];

void instrument_stdout()
{
	instrument_output[STDOUT_FILENO] = true;
	TAILQ_INIT(&entries[STDOUT_FILENO]);
}

void instrument_stderr()
{
	instrument_output[STDERR_FILENO] = true;
	TAILQ_INIT(&entries[STDERR_FILENO]);
}

static int output_buffer_size(int fd)
{
	struct entry *entry;
	size_t size = 0;

	TAILQ_FOREACH(entry, &entries[fd], entries)
		size += entry->data_len;

	return size;
}

static int get_buffer_output(int fd, char **bufer)
{
	struct entry *entry;
	size_t size = output_buffer_size(fd);
	char *buffer = calloc(1, size);
	char *ptr = buffer;

	TAILQ_FOREACH(entry, &entries[fd], entries) {
		memcpy(ptr, entry->data, entry->data_len);
		ptr += entry->data_len;
	}

	*bufer = buffer;
	return size;
}

int get_stdout_buffer(char **buffer)
{
	return get_buffer_output(STDOUT_FILENO, buffer);
}

int get_stderr_buffer(char **buffer)
{
	return get_buffer_output(STDERR_FILENO, buffer);
}

static void entry_head_deinit(struct entryhead *head)
{
	struct entry *n1 = TAILQ_FIRST(head);
	while (n1 != NULL) {
		struct entry *n2 = TAILQ_NEXT(n1, entries);
		free(n1->data);
		free(n1);
		n1 = n2;
	}
	TAILQ_INIT(head);
}

void deinstrument_stdout()
{
	instrument_output[STDOUT_FILENO] = false;
	entry_head_deinit(&entries[STDOUT_FILENO]);
}

void deinstrument_stderr()
{
	instrument_output[STDERR_FILENO] = false;
	entry_head_deinit(&entries[STDERR_FILENO]);
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

// instrument output
int __wrap_printf(const char *format, ...)
{
	int ret = 0;
	int fd = fileno(stdout);
	va_list args;
	char *buffer = NULL;
	struct entry *entry = NULL;
	va_start(args, format);

	if (!instrument_output[fd]) {
		ret = vprintf(format, args);
	} else {
		ret = vasprintf(&buffer, format, args);
		entry = calloc(1, sizeof(*entry));
		entry->data = buffer;
		entry->data_len = ret;
		TAILQ_INSERT_TAIL(&entries[fd], entry, entries);
	}

	va_end(args);
	return ret;
}

extern int __real_vfprintf(FILE *stream, const char *format, va_list args);
int __wrap_vfprintf(FILE *stream, const char *format, va_list args)
{
	int ret = 0;
	int fd = fileno(stream);
	char *buffer = NULL;
	struct entry *entry = NULL;

	if (!instrument_output[fd]) {
		ret = __real_vfprintf(stream, format, args);
	} else {
		ret = vasprintf(&buffer, format, args);
		entry = calloc(1, sizeof(*entry));
		entry->data = buffer;
		entry->data_len = ret;
		TAILQ_INSERT_TAIL(&entries[fd], entry, entries);
	}
	return ret;
}

int __wrap_fprintf(FILE *stream, const char *format, ...)
{
	int ret = 0;
	int fd = fileno(stream);
	va_list args;
	char *buffer = NULL;
	struct entry *entry = NULL;
	va_start(args, format);

	if (!instrument_output[fd]) {
		ret = __real_vfprintf(stream, format, args);
	} else {
		ret = vasprintf(&buffer, format, args);
		entry = calloc(1, sizeof(*entry));
		entry->data = buffer;
		entry->data_len = ret;
		TAILQ_INSERT_TAIL(&entries[fd], entry, entries);
	}

	va_end(args);
	return ret;
}

// libtodo instrument functions
int __wrap_todo_save_tasks(struct todo *todo)
{
	return mock_type(int);
}

void __wrap_todo_remove_task(struct todo *todo, struct task *task)
{
	return;
}

void __wrap_todo_add_task(struct todo *todo, struct task *task)
{
	return;
}

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

int __wrap_todo_load_tasks(struct todo *todo)
{
	return mock_type(int);
}

void __wrap_todo_clean_tasks(struct todo *todo)
{
	return;
}

void __wrap_destroy_todotxt(struct todo **todo)
{
	return;
}
