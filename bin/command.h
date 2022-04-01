#ifndef __COMMAND_H__
#define __COMMAND_H__

struct command {
  char *name;
  char *usage;
  char *description;
  int (*command_handle)(int, char **);
};

#endif /* end of include guard: __COMMAND_H__ */
