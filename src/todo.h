#ifndef __TODO_H__
#define __TODO_H__

#include <stddef.h>

#include "task.h"

#define container_of(ptr, type, member) (type *)( (char *)(ptr) - offsetof(type,member) )

struct todo;

struct todo_ops {
	int (*load_tasks)(struct todo*);
	int (*save_tasks)(struct todo*);
	void (*clean_tasks)(struct todo*);
	void (*add_task)(struct todo*, struct task*);
	void (*remove_task)(struct todo*, struct task*);
	void (*destroy)(struct todo**);
};

struct todo {
	struct todo_ops *ops;
#define TODO_TASK_LIST_LENGTH 50
	struct task **task_list;
	int task_counter;
};

// default todo operators
void add_task(struct todo *, struct task *);
void remove_task(struct todo *, struct task *);
void clean_tasks(struct todo *);

struct todo *create_todo();
void destroy_todo(struct todo **);
int todo_load_tasks(struct todo*);
int todo_save_tasks(struct todo*);
void todo_add_task(struct todo*, struct task*);
void todo_remove_task(struct todo*, struct task*);
void todo_clean_tasks(struct todo*);

#endif /* end of include guard: __TODO_H__ */
