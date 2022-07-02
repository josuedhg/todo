#ifndef __TODO_H__
#define __TODO_H__

#include <stddef.h>
#include <stdbool.h>

#include "task.h"

#define container_of(ptr, type, member) (type *)( (char *)(ptr) - offsetof(type,member) )

struct todo;

struct todo_driver {
	int (*add_task)(struct todo*, struct task*);
	int (*edit_task)(struct todo*, struct task*);
	struct task *(*get_task)(struct todo*, int);
	int (*remove_task)(struct todo*, struct task*);
};

struct todo {
	struct todo_driver *driver;
#define TODO_TASK_LIST_LENGTH 50
	struct task *task_list[TODO_TASK_LIST_LENGTH];
	int task_counter;
};

typedef bool (*iterator_filter)(struct task*, void*);

struct todo_iterator {
	iterator_filter filter;
	struct todo *todo;
	int index;
	void *data;
};


void todo_init(struct todo *);
int todo_add_task(struct todo*, struct task*);
int todo_edit_task(struct todo*, struct task*);
int todo_remove_task(struct todo*, struct task*);
void todo_clean_tasks(struct todo*);
struct task *todo_iterator_next(struct todo_iterator*);

#endif /* end of include guard: __TODO_H__ */
