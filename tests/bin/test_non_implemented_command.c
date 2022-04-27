#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdlib.h>
#include <string.h>

#include "todo.h"
#include "commands.h"
#include "instrument.h"

struct todo todo = {
	.task_counter = 0,
};

static struct command_descriptor desc = {32, "invalid", "non implemented", "A non implemented command" };

static struct command command = {
	.descriptor = &desc,
	.todo = &todo,
};

static void test_non_implemented_command(void **state)
{
	(void)state; /* unused */
	char *params[] = {"invalid", "1"};

	command.argv = params;
	command.argc = 2;

	assert_int_equal(command_handle(&command), -1);
}

int main(int argc, char *argv[])
{
	struct CMUnitTest tests[] = {
		cmocka_unit_test(test_non_implemented_command),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
