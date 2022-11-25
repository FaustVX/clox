#ifndef clox_vm_h
#define clox_vm_h

#include "chunk.h"
#include "value.h"
#include "table.h"

typedef struct {
  int capacity;
  Value* array;
  Value* top;
} Stack;

typedef struct {
  Chunk* chunk;
  uint8_t* ip;
  Stack stack;
  Table strings;
  Obj* objects;
} VM;

typedef enum {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} InterpretResult;

extern VM vm;

void initVM();
InterpretResult interpret(const char* source);
void push(Value value);
void setCurrent(Value value);
Value pop();
Value peek(int distance);
void freeVM();

#endif
