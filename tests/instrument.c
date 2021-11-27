#define _GNU_SOURCE
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int __wrap_access(const char *pathname, int mode)
{
	return mock_type(int);
}

FILE *__wrap_fopen(const char *restrict pathname, const char *restrict mode)
{
	return mock_ptr_type(FILE *);
}

ssize_t __wrap_getline(char **lineptr, size_t *n, FILE *stream)
{
	errno = mock_type(int);
	*lineptr = mock_ptr_type(char *);
	return mock_type(ssize_t);
}

int __wrap_fclose(FILE *stream)
{
	return 0;
}
