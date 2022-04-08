#include <stdio.h>
#include <string.h>

#include "command.h"
#include "commands.h"
#include "task.h"

static int help_handler(int argc, char **argv);
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

static int help_handler(int argc, char **argv)
{
	int i;
	printf("Usage: %s <command>\n", bin_name);
	puts("Available commands:");
	for (i = 0; commands[i] != NULL; i++) {
		printf("\t%s: %s\n", commands[i]->name, commands[i]->description);
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
		return help_handler(argc, argv);
	}

	const struct command *cmd = find_command(argv[1]);
	if (cmd == NULL) {
		printf("Unknown command: %s\n", argv[1]);
		return help_handler(argc, argv);
	}

	return cmd->command_handle(argc - 1, argv + 1);
}
