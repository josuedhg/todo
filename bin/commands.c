#include "todo.h"
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "task.h"
#include "commands.h"

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

static int add_command_handle(struct command *cmd)
{
	char *task_description = NULL;
	struct task *task = NULL;
	int ret = 0;

	if (cmd->argc < 2)
	{
		cmd->log->error("Usage: %s \n", cmd->descriptor->usage);
		return -1;
	}

	task_description = join_params(cmd->argc - 1, cmd->argv + 1, " ");
	task = create_new_task(task_description, NULL, TASK_PRIORITY_LOW);
	if (task == NULL) {
		cmd->log->error("Error: Unable to create new task: %s\n", task_description);
		ret = -1;
		goto FREE_AND_EXIT;
	}

	if (cmd->todo->driver->add_task(cmd->todo, task) < 0) {
		cmd->log->error("Error: Unable to add task: %s\n", task_description);
		ret = -1;
		goto FREE_AND_EXIT;
	}

FREE_AND_EXIT:
	free(task_description);
	return ret;
}

static int list_command_handle(struct command *cmd)
{
	struct todo_iterator *iterator = todo_get_iterator(cmd->todo, NULL, NULL);
	struct task *task = NULL;
	while ((task = todo_iterator_next(iterator)) != NULL)
		cmd->log->notify("%d. %s\n", task->id, task->name);
	free(iterator);
	return 0;
}

static int show_command_handle(struct command *cmd)
{
	if (cmd->argc < 2) {
		cmd->log->error("Usage: %s \n", cmd->descriptor->usage);
		return -1;
	}

	int task_id = 0;
	sscanf(cmd->argv[1], "%d", &task_id);

	if (task_id < 1) {
		cmd->log->error("Invalid task id format: %s\n", cmd->argv[1]);
		return -1;
	}

	struct task *task = cmd->todo->driver->get_task(cmd->todo, task_id);
	if (task == NULL) {
		cmd->log->error("Error: Unable to find task with id %d.\n", task_id);
		return -1;
	}

	cmd->log->notify("Task %d: %s\n"
			 "Project %s\n"
			 "Status %s\n",
			 task_id, task->name,
			 (task->project_name)? task->project_name : "-",
			 (task->status) ? "done" : "open");

	return 0;
}

static int delete_command_handle(struct command *cmd)
{
	if (cmd->argc < 2) {
		cmd->log->error("Usage: %s \n", cmd->descriptor->usage);
		return -1;
	}
	int task_id = 0;
	sscanf(cmd->argv[1], "%d", &task_id);

	if (task_id < 1) {
		cmd->log->error("Invalid task id format: %s\n", cmd->argv[1]);
		return -1;
	}

	if (!cmd->todo->driver->remove_task(cmd->todo, task_id)) {
		cmd->log->error("Error: Unable to remove task with id %d.\n", task_id);
		return -1;
	}

	return 0;
}

static int done_command_handle(struct command *cmd)
{
	if (cmd->argc < 2) {
		cmd->log->error("Usage: %s \n", cmd->descriptor->usage);
		return -1;
	}
	int task_id = 0;
	sscanf(cmd->argv[1], "%d", &task_id);

	if (task_id < 1) {
		cmd->log->error("Invalid task id format: %s\n", cmd->argv[1]);
		return -1;
	}

	struct task *task = cmd->todo->driver->get_task(cmd->todo, task_id);
	if (task == NULL) {
		cmd->log->error("Error: Unable to find task with id %d.\n", task_id);
		return -1;
	}

	task_set_completed(task);

	if (cmd->todo->driver->edit_task(cmd->todo, task) < 0) {
		cmd->log->error("Error: Unable to set task as done\n");
		return -1;
	}

	return 0;
}

static int reopen_command_handle(struct command *cmd)
{
	if (cmd->argc < 2) {
		cmd->log->error("Usage: %s \n", cmd->descriptor->usage);
		return -1;
	}
	int task_id = 0;
	sscanf(cmd->argv[1], "%d", &task_id);

	if (task_id < 1) {
		cmd->log->error("Invalid task id format: %s\n", cmd->argv[1]);
		return -1;
	}

	struct task *task = cmd->todo->driver->get_task(cmd->todo, task_id);
	if (task == NULL) {
		cmd->log->error("Error: Unable to find task with id %d.\n", task_id);
		return -1;
	}

	task_reopen(task);

	if (cmd->todo->driver->edit_task(cmd->todo, task) < 0) {
		cmd->log->error("Error: Unable to reopen task\n");
		return -1;
	}

	return 0;
}

static int (*command_functions[])(struct command *) = {
	add_command_handle, // ADD_COMMAND_ID 0
	list_command_handle, // LIST_COMMAND_ID 1
	show_command_handle, // SHOW_COMMAND_ID 2
	delete_command_handle, // DELETE_COMMAND_ID 3
	done_command_handle, // DONE_COMMAND_ID 4
	reopen_command_handle // REOPEN_COMMAND_ID 5
};

int command_handle(struct command *cmd)
{
	int cmd_id = cmd->descriptor->command_id;
	if (cmd_id >= (sizeof(command_functions) / sizeof(int (*)(struct command *))))
		return -1;
	return command_functions[cmd_id](cmd);
}
