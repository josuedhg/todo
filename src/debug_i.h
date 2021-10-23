#ifndef __DEBUG_I_H__
#define __DEBUG_I_H__

#ifdef UNIT_TESTING
extern void mock_assert(const int result, const char* const expression,
                        const char * const file, const int line);
#undef assert
#define assert(expression) \
    mock_assert((int)(expression), #expression, __FILE__, __LINE__);
#endif

#endif /* end of include guard: __DEBUG_I_H__ */
