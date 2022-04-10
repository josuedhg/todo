#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "task.h"
#include "debug_i.h"

struct task *create_task(const char *name, const char *project_name, int priority)
{
	if (name == NULL)
		return NULL;

	int namelen = strlen(name);
	int pnamelen = (project_name == NULL) ? 0 : strlen(project_name);

	struct task *task = calloc(1, sizeof(struct task));
	task->name = calloc(1, namelen + 1);
	task->project_name = calloc(1, pnamelen + 1);

	strncpy(task->name, name, namelen);
	if (project_name != NULL)
		strncpy(task->project_name, project_name, pnamelen);
	task->priority = priority;
	return task;
}

void task_set_completed(struct task *task)
{
	assert(task != NULL);
	task->status = TASK_STATUS_COMPLETED;
	task->completion_date = time(NULL);
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
	free((*task)->name);
	free((*task)->project_name);
	free(*task);
	*task = NULL;
}
