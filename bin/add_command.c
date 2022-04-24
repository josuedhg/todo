#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "command.h"
#include "commands.h"
#include "configure.h"
#include "todotxt.h"
#include "task.h"

static char *join_params(int argc, char **argv, char *separator)
{
	int i;
	char *joined_params;
	size_t joined_params_size;

	joined_params_size = 0;
	for (i = 0; i < argc; i++) {
		joined_params_size += strlen(argv[i]) + strlen(separator);
	}

	joined_params_size -= strlen(separator);
	joined_params = malloc(joined_params_size + 1);
	assert(joined_params);

	joined_params[0] = '\0';
	for (i = 0; i < argc; i++) {
		strcat(joined_params, argv[i]);
		if (i < argc - 1)
			strcat(joined_params, separator);
	}

	return joined_params;
}

static int add_command_handle(struct todo *todo, int argc, char **argv)
{
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s \n", add_command.usage);
		return -1;
	}

	char *task_description = NULL;
	struct task *task = NULL;
	task_description = join_params(argc - 1, argv + 1, " ");
	task = create_new_task(task_description, NULL, TASK_PRIORITY_LOW);
	if (task == NULL) {
		fprintf(stderr, "Error: Unable to create new task: %s\n", task_description);
		return -1;
	}

	todo_add_task(todo, task);

	if (todo_save_tasks(todo) < 0) {
		fprintf(stderr, "Error: Unable to save task \"%s\".\n", task->name);
		return -1;
	}

	return 0;
}

const struct command add_command = {
    .name = "add",
    .description = "Add a new task",
    .usage = "add <task description>",
    .command_handle = add_command_handle,
};
