#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "task.h"
#include "debug_i.h"

void set_completed(struct task *task)
{
	task->status = TASK_STATUS_COMPLETED;
	task->completion_date = time(NULL);
}

struct task *create_task(const char *name, const char *project_name, int priority)
{
	if (name == NULL || project_name == NULL)
		return NULL;

	int namelen = strlen(name);
	int pnamelen = strlen(project_name);
	if (namelen >= TASK_NAME_LENGTH || pnamelen >= TASK_NAME_LENGTH)
		return NULL;

	struct task *task = calloc(1, sizeof(struct task));
	task->ops = calloc(1, sizeof(struct task_ops));
	memcpy(task->name, name, namelen);
	memcpy(task->project_name, project_name, pnamelen);
	task->priority = priority;
	task->ops->set_completed = set_completed;
	return task;
}

struct task *create_new_task(const char *name, const char *project_name, int priority)
{
	struct task *task = create_task(name, project_name, priority);
	if (task == NULL)
		return NULL;
	task->creation_date = time(NULL);
	return task;
}

void destroy_task(struct task **task)
{
	if (task == NULL || *task == NULL)
		return;
	free((*task)->ops);
	free(*task);
	*task = NULL;
}

void task_set_completed(struct task *task)
{
	assert(task != NULL);
	assert(task->ops != NULL);
	assert(task->ops->set_completed != NULL);
	task->ops->set_completed(task);
}

