#ifndef __INSTRUMENT_H__
#define __INSTRUMENT_H__

#include <stdbool.h>
#include "log.h"
#include "todo.h"

void mock_log_function(const char *, ...);
void instrument_output(bool);

struct log test_logger = {
	.notify = mock_log_function,
	.error = mock_log_function,
};

int mock_todo_load_tasks(struct todo *todo);

extern struct todo_driver mock_todo_driver;

#endif /* end of include guard: __INSTRUMENT_H__ */
