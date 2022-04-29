#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include "todo.h"
#include "log.h"

#define ADD_COMMAND_ID 0
#define LIST_COMMAND_ID 1
#define SHOW_COMMAND_ID 2
#define DELETE_COMMAND_ID 3
#define DONE_COMMAND_ID 4
#define REOPEN_COMMAND_ID 5

struct command_descriptor {
	int command_id;
	char *name;
	char *usage;
	char *description;
};

struct command {
	const struct command_descriptor *descriptor;
	struct todo *todo;
	struct log *log;
	int argc;
	char **argv;
};

int command_handle(struct command *);

#endif /* end of include guard: __COMMANDS_H__ */
