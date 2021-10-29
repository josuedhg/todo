#ifndef __TODOTXT_H__
#define __TODOTXT_H__

#include <stdbool.h>
#include <time.h>

bool todotxt_get_time_from_string(const char *, time_t *);
bool todotxt_get_status(const char *, time_t *);
char *todotxt_get_project_name_from_desc(const char *);

#endif /* end of include guard: __TODOTXT_H__ */
