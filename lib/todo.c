#include <stdlib.h>
#include <assert.h>

#include "todo.h"

#include "debug_i.h"

static int todo_load_tasks(struct todo *todo)
{
	if (todo == NULL)
		return -1;
	return 0;
}

static int todo_save_tasks(struct todo *todo)
{
	if (todo == NULL)
		return -1;
	return 0;
}

static void clean_tasks(struct todo *todo)
{
	for (int i = 0; i < todo->task_counter; i++) {
		destroy_task(&todo->task_list[i]);
	}
	todo->task_counter = 0;
}

void todo_add_task(struct todo *todo, struct task *task)
{
	assert(todo != NULL && task != NULL);
	assert(todo->task_counter < TODO_TASK_LIST_LENGTH);
	todo->task_list[todo->task_counter] = task;
	todo->task_counter++;
}

static void todo_remove_task(struct todo *todo, struct task *task)
{
	assert(todo != NULL && task != NULL);
	if (todo->task_counter == 0)
		return;
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

static struct task *todo_get_task(struct todo *todo, int id)
{
	assert(todo != NULL);
	if (todo->task_counter == 0)
		return NULL;
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
	.load_tasks = todo_load_tasks,
	.save_tasks = todo_save_tasks,
	.clean_tasks = clean_tasks,
	.add_task = todo_add_task,
	.remove_task = todo_remove_task,
	.get_task = todo_get_task,
};

void todo_init(struct todo *todo)
{
	todo->task_counter = 0;
	*todo->driver = driver;
}

void todo_clean_tasks(struct todo *todo)
{
	assert(todo != NULL);
	if (todo->task_counter == 0)
		return;
	todo->driver->clean_tasks(todo);
}

struct task *todo_iterator_next(struct todo_iterator *iterator)
{
	assert(iterator != NULL);
	struct task *task = NULL;
	while (iterator->index < iterator->todo->task_counter) {
		struct task *tmp = iterator->todo->task_list[iterator->index];
		iterator->index++;
		if (iterator->filter == NULL || iterator->filter(tmp, iterator->data)) {
			task = tmp;
			break;
		}
	}
	return task;
}
