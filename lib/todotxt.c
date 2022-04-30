#define _XOPEN_SOURCE
#define _GNU_SOURCE
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include "todotxt.h"
#include "todotxt_i.h"
#include "task.h"
#include "debug_i.h"

#define STATUS_FORMAT_LENGTH 12
#define DUEDATE_FORMAT_LENGTH 14
#define PRIORITY_FORMAT_LENGTH 3
#define DATE_LENGHT 11

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

bool todotxt_get_duedate_from_desc(const char *str, time_t *time)
{
	assert(str != NULL && time != NULL);
	int str_len = strlen(str);
	if (str_len < DUEDATE_FORMAT_LENGTH)
		return false;
	int offset = 5;
	char *duematch = strstr(str, " due:");
	if (duematch == NULL) {
		duematch = strstr(str, "due:");
		offset = 4;
		if (duematch != str || duematch ==  NULL)
			return false;
	}
	if (!todotxt_get_time_from_string(&duematch[offset], time))
		return false;
	return true;
}

char *todotxt_get_project_name_from_desc(const char *str)
{
	assert(str != NULL);
	int str_len = strlen(str);
	bool buffering = false;
	int buffer_index = 0;
	int i = 0;
	const char *buffer = NULL;
	for (i = 0; i < str_len; i++)
	{
		if (str[i] == '+' && (i == 0 || str[i - 1] == ' ') && isalpha(str[i + 1])) {
			buffering = true;
			continue;
		}

		if (buffering && str[i] == ' ')
			break;

		if (buffering && buffer == NULL)
			buffer = &str[i];

		if (buffer != NULL)
			buffer_index++;
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

struct task *create_task_from_todotxt(const char *todoline)
{
	if (todoline == NULL)
		return NULL;
	struct task *task = calloc(1, sizeof(struct task));
	int index = 0;

	time_t completed = 0;
	if (todotxt_get_status(&todoline[index], &completed)) {
		task->status = TASK_STATUS_COMPLETED;
		task->completion_date = completed;
		index += 2;
	}

	task->priority = todotxt_get_priority(&todoline[index]);
	if (task->priority == NO_PRIORITY_FORMAT) {
		task->priority = TASK_PRIORITY_LOW;
	} else {
		index += 4;
	}

	if (task->status == TASK_STATUS_COMPLETED)
		index += 11;

	time_t created = 0;
	if (todotxt_get_time_from_string(&todoline[index], &created)) {
		task->creation_date = created;
		index += 11;
	}

	time_t duedate = 0;
	if (todotxt_get_duedate_from_desc(&todoline[index], &duedate)) {
		task->due_date = duedate;
	}

	char *project_name = todotxt_get_project_name_from_desc(&todoline[index]);
	if (project_name != NULL)
		task->project_name = project_name;

	task->name = calloc(1, strlen(&todoline[index]) + 1);
	memcpy(task->name, &todoline[index], strlen(&todoline[index]));

	return task;
}

#define DATE_FORMAT "%Y-%m-%d"
int create_todotxt_line_from_task(struct task *task, char **buffer)
{
	if (task == NULL || buffer == NULL)
		return -1;
	char *line = NULL;
	char create_date[DATE_LENGHT] = { 0 };
	char due_date[DATE_LENGHT] = { 0 };
	char completion_date[DATE_LENGHT] = { 0 };

	char *line_header_format = (task->status == TASK_STATUS_COMPLETED)? "x (%c) %s %s":"(%c) %s";
	char line_header[100] = { 0 };

	if (task->status == TASK_STATUS_COMPLETED) {
		strftime(completion_date, DATE_LENGHT, DATE_FORMAT, localtime(&task->completion_date));
		strftime(create_date, DATE_LENGHT, DATE_FORMAT, localtime(&task->creation_date));
		sprintf(line_header, line_header_format, task->priority + 'A', create_date, completion_date);
	} else {
		strftime(create_date, DATE_LENGHT, DATE_FORMAT, localtime(&task->creation_date));
		sprintf(line_header, line_header_format, task->priority + 'A', create_date, "");
	}

	int line_length = strlen(line_header) + strlen(task->name) + 1;

	if (strstr(task->name, "+") == NULL && task->project_name != NULL)
		line_length += strlen(task->project_name) + 2; // +2 for the space and the +

	if (task->due_date != 0) {
		strftime(due_date, DATE_LENGHT, DATE_FORMAT, localtime(&task->due_date));
		line_length += strlen(due_date) + 5; // +5 for the space, the : and the due:
	}

	line = calloc(1, line_length * 2);
	sprintf(line, "%s %s", line_header, task->name);

	if (strstr(task->name, "+") == NULL && task->project_name != NULL)
		sprintf(line, "%s +%s", line, task->project_name);

	if (task->due_date != 0 && strstr(task->name, "due:") == NULL)
		sprintf(line, "%s due:%s", line, due_date);
	*buffer = line;
	return line_length;
}

static int load_tasks(struct todo *todo)
{
	struct todotxt *todotxt = container_of(todo, struct todotxt, todo);
	FILE *file = fopen(todotxt->filename, "r");
	int ret = 0;
	int index = 1;
	if (file == NULL)
		return -1;
	size_t lines_len = 0;
	char *line = NULL;
	ssize_t line_size = 0;
	while ((line_size = getline(&line, &lines_len, file)) != -1) {
		if (line[line_size - 1] == '\n')
			line[line_size - 1] = '\0';
		struct task *task = create_task_from_todotxt(line);
		task->id = index++;
		todo_add_task(todo, task);
	}
	free(line);
	if (errno == EINVAL)
		ret = -1;
	fclose(file);
	return ret;
}

static int save_tasks(struct todo *todo)
{
	struct todotxt *todotxt = container_of(todo, struct todotxt, todo);
	FILE *file = fopen(todotxt->filename, "w");
	if (file == NULL)
		return -1;
	struct task **task = todo->task_list;
	for (int i = 0; i < todo->task_counter; i++) {
		char *line = NULL;
		int line_length = create_todotxt_line_from_task(task[i], &line);
		if (i < todo->task_counter - 1) {
			line_length = line_length + 1;
			line = realloc(line, line_length);
			line[line_length - 1] = '\n';
		}
		if (fwrite(line, line_length, 1, file) != 1) {
			free(line);
			fclose(file);
			return -1;
		}
		free(line);
	}
	fclose(file);
	return 0;
}

void destroy_todotxt(struct todo **todo)
{
	struct todotxt *todotxt = container_of(*todo,
					       struct todotxt,
					       todo);
	free(todotxt->filename);
	free(todotxt);
	*todo = NULL;
}

struct todo_driver todotxt_driver;

struct todo *create_todotxt(char *filename)
{
	if (filename == NULL)
		return NULL;
	if (access(filename, R_OK | W_OK) != 0)
		return NULL;
	struct todotxt *todotxt = calloc(1, sizeof(struct todotxt));
	todotxt->todo.driver = &todotxt_driver;
	todo_init(&todotxt->todo);
	todotxt->todo.driver->load_tasks = load_tasks;
	todotxt->todo.driver->save_tasks = save_tasks;
	todotxt->filename = calloc(1, strlen(filename) + 1);
	strcpy(todotxt->filename, filename);
	return &todotxt->todo;
}
