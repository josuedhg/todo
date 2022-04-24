#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

#include "command.h"
#include "commands.h"
#include "configure.h"
#include "todotxt.h"

int list_command_handler(struct todo *todo, int argc, char **argv)
{
	for (int i = 0; i < todo->task_counter; i++) {
		printf("%d. %s\n", i + 1, todo->task_list[i]->name);
	}

	return 0;
}

const struct command list_command = {
	.name = "list",
	.description = "List all available commands",
	.usage = "list",
	.command_handle = list_command_handler,
};
