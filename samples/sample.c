#include <stdio.h>

#include "task.h"

int main(void)
{
	char time_str[50];
	struct tm *tm = NULL;
	struct task *task = create_new_task("my task", "my project", TASK_PRIORITY_LOW);

	tm = localtime(&task->creation_date);
	strftime(time_str, sizeof(time_str), "%a %b %d %H:%M:%S %Y", tm);
	printf("task name: %s\n"
	       "project name: %s\n"
	       "prority: %d\n"
	       "status: %d\n"
	       "creation date: %s\n",
	       task->name,
	       task->project_name,
	       task->priority,
	       task->status,
	       time_str);
	return 0;
}
