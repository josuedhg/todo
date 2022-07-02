#define _GNU_SOURCE
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

bool real_fwrite = true;

void wrap_fwrite()
{
    real_fwrite = false;
}

void reset_fwrite()
{
    real_fwrite = true;
}

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

extern size_t __real_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t __wrap_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	int ret = 0;
	if (real_fwrite)
		return __real_fwrite(ptr, size, nmemb, stream);

	ret = mock_type(int);
	if (ret == nmemb) {
		check_expected(size);
		check_expected(ptr);
	}
	return ret;
}

int __wrap_fclose(FILE *stream)
{
	return 0;
}
