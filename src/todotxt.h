#ifndef __TODOTXT_H__
#define __TODOTXT_H__

#include "todo.h"

struct todo *create_todotxt(char *);
void destroy_todotxt(struct todo **);

#endif /* end of include guard: __TODOTXT_H__ */
