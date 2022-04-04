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

#include "todo.h"

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
