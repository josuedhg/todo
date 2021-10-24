#include <stdlib.h>
#include <assert.h>

#include "todo.h"

#include "debug_i.h"

static void load_tasks(struct todo *todo)
{
}

static void save_tasks(struct todo *todo)
{
}

void clean_tasks(struct todo *todo)
{
	for (int i = 0; i < todo->task_counter; i++) {
		destroy_task(&todo->task_list[i]);
	}
	todo->task_counter = 0;
}

void add_task(struct todo *todo, struct task *task)
{
	todo->task_list[todo->task_counter] = task;
	todo->task_counter++;
}

void remove_task(struct todo *todo, struct task *task)
{
	int index = 0;
	for (; index < todo->task_counter; index++) {
		if (todo->task_list[index] == task)
			break;
	}
	if (index == todo->task_counter)
		return;
	todo->task_counter--;
	for (; index < todo->task_counter; index++) {
		todo->task_list[index] = todo->task_list[index + 1];
	}
	todo->task_list[index] = NULL;
}

struct todo *create_todo()
{
	struct todo *todo = calloc(1, sizeof(struct todo));
	todo->ops = calloc(1, sizeof(struct todo_ops));
	todo->task_list = calloc(1, sizeof(struct task *) * TODO_TASK_LIST_LENGTH);
	todo->task_counter = 0;
	todo->ops->load_tasks = load_tasks;
	todo->ops->save_tasks = save_tasks;
	todo->ops->add_task = add_task;
	todo->ops->remove_task = remove_task;
	todo->ops->clean_tasks = clean_tasks;
	return todo;
}

void destroy_todo(struct todo **todo)
{
	if (todo == NULL || *todo == NULL)
		return;
	free((*todo)->ops);
	free((*todo)->task_list);
	free(*todo);
	*todo = NULL;
}

void todo_load_tasks(struct todo *todo)
{
	assert(todo != NULL);
	todo->ops->load_tasks(todo);
}

void todo_save_tasks(struct todo *todo)
{
	assert(todo != NULL);
	todo->ops->save_tasks(todo);
}

void todo_add_task(struct todo *todo, struct task *task)
{
	assert(todo != NULL && task != NULL);
	assert(todo->task_counter < TODO_TASK_LIST_LENGTH);
	todo->ops->add_task(todo, task);
}

void todo_remove_task(struct todo *todo, struct task *task)
{
	assert(todo != NULL && task != NULL);
	if (todo->task_counter == 0)
		return;
	todo->ops->remove_task(todo, task);
}

void todo_clean_tasks(struct todo *todo)
{
	assert(todo != NULL);
	if (todo->task_counter == 0)
		return;
	todo->ops->clean_tasks(todo);
}
