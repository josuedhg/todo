#ifndef __TASK_H__
#define __TASK_H__

#include <time.h>

struct task {
	char *name;
	char *project_name;
#define TASK_STATUS_OPEN 0
#define TASK_STATUS_COMPLETED 1
	int status;
#define TASK_PRIORITY_HIGH 0
#define TASK_PRIORITY_MEDIUM 1
#define TASK_PRIORITY_LOW 2
	int priority;
	time_t creation_date;
	time_t completion_date;
	time_t due_date;
};

struct task *create_task(const char *, const char *, int);
struct task *create_new_task(const char *, const char *, int);
void destroy_task(struct task **);
void task_set_completed(struct task *);
void task_reopen(struct task *);

#endif /* end of include guard: __TASK_H__ */
