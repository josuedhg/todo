#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "command.h"
#include "commands.h"
#include "todotxt.h"

static int reopen_command_handle(struct todo *todo, int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s \n", reopen_command.usage);
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

	task_reopen(task);

	if (todo_save_tasks(todo) < 0) {
		fprintf(stderr, "Error: Unable to save task \"%s\".\n", task->name);
		return -1;
	}

	return 0;
}

const struct command reopen_command = {
    .name = "reopen",
    .description = "Set task as open.",
    .usage = "reopen [task id]",
    .command_handle = reopen_command_handle,
};
