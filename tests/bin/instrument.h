#ifndef __INSTRUMENT_H__
#define __INSTRUMENT_H__

#include "log.h"

void instrument_stdout();
void instrument_stderr();

int get_stdout_buffer(char **buffer);
int get_stderr_buffer(char **buffer);

void deinstrument_stdout();
void deinstrument_stderr();

void mock_log_function(const char *, ...);

struct log logger = {
	.notify = mock_log_function,
	.error = mock_log_function,
};

#endif /* end of include guard: __INSTRUMENT_H__ */
