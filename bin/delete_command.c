#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "command.h"
#include "commands.h"
#include "configure.h"
#include "todotxt.h"

static int delete_command_handle(struct todo *todo, int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s \n", delete_command.usage);
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

	todo_remove_task(todo, task);

	if (todo_save_tasks(todo) < 0) {
		fprintf(stderr, "Error: Unable to save task \"%s\".\n", task->name);
		return -1;
	}

	return 0;
}

const struct command delete_command = {
    .name = "delete",
    .description = "Delete a task",
    .usage = "delete [task id]",
    .command_handle = delete_command_handle,
};
