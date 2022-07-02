#include <stdlib.h>
#include <assert.h>

#include "todo.h"

#include "debug_i.h"

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

int todo_edit_task(struct todo *todo, struct task *task)
{
	if (todo == NULL || task == NULL)
		return -1;

	struct task *current = todo_get_task(todo, task->id);
	if (current == NULL)
		return -1;
	*current = *task;
	return task->id;
}

void todo_clean_tasks(struct todo *todo)
{
	assert(todo != NULL);
	if (todo->task_counter == 0)
		return;
	for (int i = 0; i < todo->task_counter; i++) {
		destroy_task(&todo->task_list[i]);
	}
	todo->task_counter = 0;
}

int todo_add_task(struct todo *todo, struct task *task)
{
	if (todo == NULL || task == NULL)
		return -1;

	if (todo->task_counter >= TODO_TASK_LIST_LENGTH)
		return -1;

	int id = todo->task_counter;
	todo->task_list[id] = task;
	todo->task_counter++;
	return id;
}

struct task *todo_remove_task(struct todo *todo, int id)
{
	if (todo == NULL)
		return NULL;
	if (todo->task_counter == 0)
		return NULL;
	struct task *task = NULL;
	int index = 0;
	for (; index < todo->task_counter; index++) {
		if (todo->task_list[index]->id == id)
			break;
	}
	task = todo->task_list[index];
	if (index >= todo->task_counter)
		return NULL;
	todo->task_counter--;
	for (; index < todo->task_counter; index++) {
		todo->task_list[index] = todo->task_list[index + 1];
	}
	todo->task_list[index] = NULL;
	return task;
}

struct todo_driver driver = {
	.edit_task = todo_edit_task,
	.add_task = todo_add_task,
	.remove_task = todo_remove_task,
	.get_task = todo_get_task,
};

void todo_init(struct todo *todo)
{
	todo->task_counter = 0;
	*todo->driver = driver;
}

struct todo_iterator {
	iterator_filter filter;
	struct todo *todo;
	int index;
	void *data;
};

struct todo_iterator *todo_get_iterator(struct todo *todo, iterator_filter filter, void *data)
{
	struct todo_iterator *iterator = calloc(1, sizeof(struct todo_iterator));
	iterator->filter = filter;
	iterator->todo = todo;
	iterator->index = 0;
	iterator->data = data;
	return iterator;
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
