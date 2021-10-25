#define _XOPEN_SOURCE
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "todotxt.h"

#define STATUS_FORMAT_LENGTH 12

bool get_time_from_string(const char *str, time_t *time)
{
	if (str == NULL || time == NULL)
		return false;
	struct tm tm = { 0 };
	if (strptime(str, "%Y-%m-%d", &tm) == NULL)
		return false;

	*time = mktime(&tm);
	return true;
}

bool todotxt_get_status(const char *str, time_t *time)
{
	if (str == NULL || time == NULL)
		return false;
	int str_len = strlen(str);
	if (str_len < STATUS_FORMAT_LENGTH)
		return false;

	int index = 0;
	if (str[index] != 'x' || str[index + 1] != ' ')
		return false;
	index += 2;
	if (str[index] == '(' && str[index + 2] == ')')
		index += 4;
	if (!get_time_from_string(&str[index], time))
		return false;
	return true;
}
