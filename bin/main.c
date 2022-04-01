#include <stdio.h>
#include <string.h>

#include "command.h"
#include "task.h"

static const struct command *commands[] = {
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

#ifdef TESTING_MODE
int test_main(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
	if (argc < 2) {
		printf("Usage: %s <command>\n", argv[0]);
		return -1;
	}

	const struct command *cmd = find_command(argv[1]);
	if (cmd == NULL) {
		printf("Unknown command: %s\n", argv[1]);
		return -1;
	}

	return cmd->command_handle(argc - 1, argv + 1);
}
