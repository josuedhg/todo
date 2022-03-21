#ifndef __TODOTXT_I_H__
#define __TODOTXT_I_H__

#include <stdbool.h>
#include <time.h>

#include "todo.h"

struct todotxt {
	struct todo todo;
	char *filename;
};

#define NO_PRIORITY_FORMAT -1

bool todotxt_get_time_from_string(const char *, time_t *);
bool todotxt_get_status(const char *, time_t *);
bool todotxt_get_duedate_from_desc(const char *, time_t *);
char *todotxt_get_project_name_from_desc(const char *);
int todotxt_get_priority(const char *);
struct task *create_task_from_todotxt(const char *);
int create_todotxt_line_from_task(struct task *, char **);



#endif /* end of include guard: __TODOTXT_I_H__ */
