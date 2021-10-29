#define _XOPEN_SOURCE
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#include "todotxt.h"
#include "task.h"
#include "debug_i.h"

#define STATUS_FORMAT_LENGTH 12
#define PRIORITY_FORMAT_LENGTH 3

bool todotxt_get_time_from_string(const char *str, time_t *time)
{
	assert(str != NULL && time != NULL);
	struct tm tm = { 0 };
	if (strptime(str, "%Y-%m-%d", &tm) == NULL)
		return false;

	*time = mktime(&tm);
	return true;
}

bool todotxt_get_status(const char *str, time_t *time)
{
	assert(str != NULL && time != NULL);
	int str_len = strlen(str);
	if (str_len < STATUS_FORMAT_LENGTH)
		return false;

	int index = 0;
	if (str[index] != 'x' || str[index + 1] != ' ')
		return false;
	index += 2;
	if (str[index] == '(' && str[index + 2] == ')')
		index += 4;
	if (!todotxt_get_time_from_string(&str[index], time))
		return false;
	return true;
}

char *todotxt_get_project_name_from_desc(const char *str)
{
	assert(str != NULL);
	int str_len = strlen(str);
	bool buffering = false;
	int buffer_index = 0;
	char buffer[TASK_NAME_LENGTH] = { 0 };
	for (int i = 0; i < str_len; i++)
	{
		if (str[i] == '+' && (i == 0 || str[i - 1] == ' ') && isalpha(str[i + 1])) {
			buffering = true;
			continue;
		}
		if (buffering && str[i] == ' ') {
			break;
		}
		if (buffering)
			buffer[buffer_index++] = str[i];
	}
	if (buffer_index == 0)
		return NULL;
	char *project_name = calloc(1, buffer_index + 1);
	memcpy(project_name, buffer, buffer_index);
	return project_name;
}

int todotxt_get_priority(const char *str)
{
	assert(str != NULL);
	int str_len = strlen(str);
	if (str_len < PRIORITY_FORMAT_LENGTH)
		return NO_PRIORITY_FORMAT;
	if (str[0] != '(' || str[2] != ')' || !isalpha(str[1]) || !isupper(str[1]))
		return NO_PRIORITY_FORMAT;
	return str[1] - 'A';
}
