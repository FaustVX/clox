#ifndef clox_common_h
#define clox_common_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define DEBUG_PRINT_CODE
#define DEBUG_TRACE_EXECUTION

#define CHECK_NULL(type, name, func, errorcode, ...)\
type name = func;\
do if (name == NULL) {\
  fprintf(stderr, __VA_ARGS__);\
  exit(errorcode);\
} while (false)

#define CHECK_COND(type, name, func, cond, errorcode, ...)\
type name = func;\
do if(name cond) {\
  fprintf(stderr, __VA_ARGS__);\
  exit(errorcode);\
} while (false)

#endif
