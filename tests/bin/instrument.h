#ifndef __INSTRUMENT_H__
#define __INSTRUMENT_H__

#include "log.h"

void mock_log_function(const char *, ...);

struct log test_logger = {
	.notify = mock_log_function,
	.error = mock_log_function,
};

#endif /* end of include guard: __INSTRUMENT_H__ */
