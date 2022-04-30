#include <stdlib.h>
#include <assert.h>

#include "todo.h"

#include "debug_i.h"

static int load_tasks(struct todo *todo)
{
	return 0;
}

static int save_tasks(struct todo *todo)
{
	return 0;
}

static void clean_tasks(struct todo *todo)
{
	for (int i = 0; i < todo->task_counter; i++) {
		destroy_task(&todo->task_list[i]);
	}
	todo->task_counter = 0;
}

static void add_task(struct todo *todo, struct task *task)
{
	todo->task_list[todo->task_counter] = task;
	todo->task_counter++;
}

static void remove_task(struct todo *todo, struct task *task)
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

struct task *get_task(struct todo *todo, int id)
{
	struct task *task = NULL;
	for (int i = 0; i < todo->task_counter; i++) {
		struct task *current = todo->task_list[i];
		if (current->id == id) {
			task = current;
			break;
		}
	}
	return task;
}

struct todo_driver driver = {
	.load_tasks = load_tasks,
	.save_tasks = save_tasks,
	.clean_tasks = clean_tasks,
	.add_task = add_task,
	.remove_task = remove_task,
	.get_task = get_task,
};

void todo_init(struct todo *todo)
{
	todo->task_counter = 0;
	*todo->driver = driver;
}

int todo_load_tasks(struct todo *todo)
{
	if (todo == NULL)
		return -1;
	return todo->driver->load_tasks(todo);
}

int todo_save_tasks(struct todo *todo)
{
	if (todo == NULL)
		return -1;
	return todo->driver->save_tasks(todo);
}

void todo_add_task(struct todo *todo, struct task *task)
{
	assert(todo != NULL && task != NULL);
	assert(todo->task_counter < TODO_TASK_LIST_LENGTH);
	todo->driver->add_task(todo, task);
}

void todo_remove_task(struct todo *todo, struct task *task)
{
	assert(todo != NULL && task != NULL);
	if (todo->task_counter == 0)
		return;
	todo->driver->remove_task(todo, task);
}

void todo_clean_tasks(struct todo *todo)
{
	assert(todo != NULL);
	if (todo->task_counter == 0)
		return;
	todo->driver->clean_tasks(todo);
}

struct task *todo_get_task(struct todo *todo, int id)
{
	assert(todo != NULL);
	if (todo->task_counter == 0)
		return NULL;
	return todo->driver->get_task(todo, id);
}
