#ifndef __LOG_H__
#define __LOG_H__

struct log {
	void (*notify)(const char *, ...);
	void (*error)(const char *, ...);
};

#endif /* end of include guard: __LOG_H__ */
