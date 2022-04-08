#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

#include "command.h"
#include "commands.h"
#include "configure.h"
#include "todotxt.h"

int list_command_handler(int argc, char **argv)
{
	char *todotxt_config_dir = NULL;
	int ret = 0;
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

	for (int i = 0; i < todo->task_counter; i++) {
		printf("%d. %s\n", i + 1, todo->task_list[i]->name);
	}

cleanup_and_exit:
	todo_clean_tasks(todo);
	destroy_todotxt(&todo);
	free(todotxt_config_dir);
	return ret;
}

const struct command list_command = {
	.name = "list",
	.description = "List all available commands",
	.usage = "list",
	.command_handle = list_command_handler,
};
