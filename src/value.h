#ifndef clox_value_h
#define clox_value_h

#include "common.h"

typedef double Value;

typedef struct {
  int count;
  int capacity;
  Value* values;
} ValueArray;

void initValueArray(ValueArray* value);
void freeValueArray(ValueArray* value);
void writeValueArray(ValueArray* value, Value array);
void printValue(Value value);

#endif