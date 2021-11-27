#ifndef __TODOTXT_H__
#define __TODOTXT_H__

#include <stdbool.h>
#include <time.h>

#include "todo.h"

#define NO_PRIORITY_FORMAT -1

struct todotxt {
	struct todo todo;
	char *filename;
};

bool todotxt_get_time_from_string(const char *, time_t *);
bool todotxt_get_status(const char *, time_t *);
bool todotxt_get_duedate_from_desc(const char *, time_t *);
char *todotxt_get_project_name_from_desc(const char *);
int todotxt_get_priority(const char *);
struct task *create_task_from_todotxt(const char *);

struct todo *create_todotxt(char *filename);

#endif /* end of include guard: __TODOTXT_H__ */
