#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "command.h"
#include "commands.h"
#include "configure.h"
#include "todotxt.h"
#include "task.h"

#ifdef TESTING_MODE
#define main test_main
#endif

static const struct command *commands[] = {
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

static int help(char *bin_name)
{
	int i;
	fprintf(stderr, "Usage: %s <command>\n"
			"Available commands:\n"
			"\thelp: Display this help message\n",
			bin_name);
	for (i = 0; commands[i] != NULL; i++) {
		fprintf(stderr, "\t%s: %s\n", commands[i]->name, commands[i]->description);
	}
	return -1;
}

static struct todo *get_todo()
{
	char *todotxt_filepath = NULL;
	struct todo *todo = NULL;
	asprintf(&todotxt_filepath, "%s/%s", getenv("HOME"), TODOTXT_FILE_NAME);

	todo = create_todotxt(todotxt_filepath);
	if (todo == NULL) {
		fprintf(stderr, "Error: Unable to read %s file.\n", todotxt_filepath);
		goto FREE_AND_EXIT;
	}

	if (todo_load_tasks(todo) < 0) {
		fprintf(stderr, "Error: Unable to load tasks from %s file.\n", todotxt_filepath);
		destroy_todotxt(&todo);
		todo = NULL;
	}

FREE_AND_EXIT:
	free(todotxt_filepath);
	return todo;
}

int main(int argc, char **argv)
{
	if (argc < 2 || strcmp(argv[1], "help") == 0) {
		return help(argv[0]);
	}

	const struct command *cmd = NULL;
	struct todo *todo = NULL;
	int ret = 0;

	cmd = find_command(argv[1]);
	if (cmd == NULL) {
		fprintf(stderr, "Unknown command: %s\n", argv[1]);
		return help(argv[0]);
	}

	if ((todo = get_todo()) == NULL)
		return -1;

	ret = cmd->command_handle(todo, argc - 1, argv + 1);
	todo_clean_tasks(todo);
	destroy_todotxt(&todo);
	return ret;
}
