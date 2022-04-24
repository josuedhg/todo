#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "command.h"
#include "commands.h"
#include "configure.h"
#include "todotxt.h"

int show_command_handler(struct todo *todo, int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s \n", show_command.usage);
		return -1;
	}

	int task_id = 0;
	sscanf(argv[1], "%d", &task_id);

	if (task_id < 1) {
		fprintf(stderr, "Invalid task id format: %s\n", argv[1]);
		return -1;
	}

	struct task *task = NULL;
	for (int i = 0; i < todo->task_counter; i++) {
		if (i + 1 == task_id) {
			task = todo->task_list[i];
			break;
		}
	}

	if (task == NULL) {
		fprintf(stderr, "Error: Unable to find task with id %d.\n", task_id);
		return -1;
	}

	printf( "Task %d: %s\n"
		"Project %s\n"
		"Status %s\n",
		task_id, task->name,
		(task->project_name)? task->project_name : "-",
		(task->status) ? "done" : "open");

	return 0;
}

const struct command show_command = {
	.name = "show",
	.description = "Show task details",
	.usage = "show [task_id]",
	.command_handle = show_command_handler,
};
