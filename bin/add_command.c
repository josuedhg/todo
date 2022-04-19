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

static int add_command_handle(int argc, char **argv)
{
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s \n", add_command.usage);
		return -1;
	}

	char *todotxt_config_dir = NULL;
	int ret = 0;
	char *task_description = NULL;
	struct task *task = NULL;
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

	task_description = join_params(argc - 1, argv + 1, " ");
	task = create_new_task(task_description, NULL, TASK_PRIORITY_LOW);
	if (task == NULL) {
		fprintf(stderr, "Error: Unable to create new task: %s\n", task_description);
		ret = -1;
		goto cleanup_and_exit;
	}

	todo_add_task(todo, task);

	if (todo_save_tasks(todo) < 0) {
		fprintf(stderr, "Error: Unable to save tasks to %s file.\n", todotxt_config_dir);
		ret = -1;
		goto cleanup_and_exit;
	}

cleanup_and_exit:
	todo_clean_tasks(todo);
	destroy_todotxt(&todo);
	free(todotxt_config_dir);
	return ret;
}

const struct command add_command = {
    .name = "add",
    .description = "Add a new task",
    .usage = "add <task description>",
    .command_handle = add_command_handle,
};
