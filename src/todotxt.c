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
#include "task.h"
#include "debug_i.h"

#define STATUS_FORMAT_LENGTH 12
#define DUEDATE_FORMAT_LENGTH 14
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
	if (project_name != NULL) {
		memcpy(task->project_name, project_name, strlen(project_name));
		free(project_name);
	}

	memcpy(task->name, &todoline[index], strlen(&todoline[index]));

	return task;
}

static int load_tasks(struct todo *todo)
{
	struct todotxt *todotxt = container_of(todo, struct todotxt, todo);
	FILE *file = fopen(todotxt->filename, "r");
	int ret = 0;
	if (file == NULL)
		return -1;
	size_t lines_len = 0;
	char *line = NULL;
	ssize_t line_size = 0;
	while ((line_size = getline(&line, &lines_len, file)) != -1) {
		struct task *task = create_task_from_todotxt(line);
		todo_add_task(todo, task);
	}
	free(line);
	if (errno == EINVAL)
		ret = -1;
	fclose(file);
	return ret;
}

static void save_tasks(struct todo *todo)
{
}

static void destroy(struct todo **todo)
{
	struct todotxt *todotxt = container_of(*todo,
					       struct todotxt,
					       todo);
	free(todotxt->todo.ops);
	free(todotxt->todo.task_list);
	free(todotxt);
	*todo = NULL;
}

struct todo *create_todotxt(char *filename)
{
	if (filename == NULL)
		return NULL;
	if (access(filename, R_OK | W_OK) != 0)
		return NULL;
	struct todotxt *todotxt = calloc(1, sizeof(struct todotxt));
	todotxt->todo.ops = calloc(1, sizeof(struct todo_ops));
	todotxt->todo.task_list = calloc(1, sizeof(struct task *) * TODO_TASK_LIST_LENGTH);
	todotxt->todo.task_counter = 0;
	todotxt->todo.ops->load_tasks = load_tasks;
	todotxt->todo.ops->save_tasks = save_tasks;
	todotxt->todo.ops->add_task = add_task;
	todotxt->todo.ops->remove_task = remove_task;
	todotxt->todo.ops->clean_tasks = clean_tasks;
	todotxt->todo.ops->destroy = destroy;
	todotxt->filename = filename;
	return &todotxt->todo;
}
