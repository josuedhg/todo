#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "command.h"
#include "commands.h"
#include "configure.h"
#include "todotxt.h"
#include "task.h"

static int help_handler(struct todo *todo, int argc, char **argv);
static char *bin_name = NULL;

const struct command help_command = {
	.name = "help",
	.description = "Display this help message",
	.usage = "help",
	.command_handle = help_handler,
};

static const struct command *commands[] = {
	&help_command,
	&list_command,
	&show_command,
	&add_command,
	&delete_command,
	&done_command,
	&reopen_command,
	NULL
};

const struct command *find_command(char *name)
{
	int i;
	for (i = 0; commands[i] != NULL; i++) {
		if (strcmp(name, commands[i]->name) == 0) {
			return commands[i];
		}
	}
	return NULL;
}

static int help_handler(struct todo *todo, int argc, char **argv)
{
	(void) todo;
	(void) argc;
	(void) argv;
	int i;
	fprintf(stderr, "Usage: %s <command>\n"
			"Available commands:\n",
			bin_name);
	for (i = 0; commands[i] != NULL; i++) {
		fprintf(stderr, "\t%s: %s\n", commands[i]->name, commands[i]->description);
	}
	return -1;
}

#ifdef TESTING_MODE
int test_main(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
	bin_name = argv[0];
	if (argc < 2) {
		return help_handler(NULL, argc, argv);
	}

	const struct command *cmd = find_command(argv[1]);
	if (cmd == NULL) {
		fprintf(stderr, "Unknown command: %s\n", argv[1]);
		return help_handler(NULL, argc, argv);
	}

	int ret = 0;
	char *todotxt_config_dir = NULL;
	asprintf(&todotxt_config_dir, "%s/%s", getenv("HOME"), TODOTXT_FILE_NAME);

	struct todo *todo = create_todotxt(todotxt_config_dir);
	if (todo == NULL) {
		fprintf(stderr, "Error: Unable to read %s file.\n", todotxt_config_dir);
		ret = -1;
		goto EXIT_AND_CLEAN;
	}

	if (todo_load_tasks(todo) < 0) {
		fprintf(stderr, "Error: Unable to load tasks from %s file.\n", todotxt_config_dir);
		ret = -1;
		goto EXIT_AND_CLEAN;
	}

	ret = cmd->command_handle(todo, argc - 1, argv + 1);
EXIT_AND_CLEAN:
	todo_clean_tasks(todo);
	destroy_todotxt(&todo);
	free(todotxt_config_dir);
	return ret;
}
