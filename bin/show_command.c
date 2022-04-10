#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "command.h"
#include "commands.h"
#include "configure.h"
#include "todotxt.h"

int show_command_handler(int argc, char **argv)
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
	int ret = 0;
	char *todotxt_config_dir = NULL;
	asprintf(&todotxt_config_dir, "%s/%s", getenv("HOME"), TODOTXT_FILE_NAME);

	struct todo *todo = create_todotxt(todotxt_config_dir);
	if (todo == NULL) {
		fprintf(stderr, "Error: Unable to read %s file.\n", todotxt_config_dir);
		free(todotxt_config_dir);
		return -1;
	}

	if (todo_load_tasks(todo) < 0) {
		fprintf(stderr, "Error: Unable to load tasks from %s file.\n", todotxt_config_dir);
		ret = -1;
		goto cleanup_and_exit;
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
		ret = -1;
		goto cleanup_and_exit;
	}

	printf("Task %d: %s\n", task_id, task->name);
	printf("Project %s\n", (task->project_name)? task->project_name : "-");
	printf("Status %s\n", (task->status) ? "done" : "open");

cleanup_and_exit:
	todo_clean_tasks(todo);
	destroy_todotxt(&todo);
	free(todotxt_config_dir);
	return ret;
}

const struct command show_command = {
	.name = "show",
	.description = "Show task details",
	.usage = "show [task_id]",
	.command_handle = show_command_handler,
};
