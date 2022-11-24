#ifndef clox_vm_h
#define clox_vm_h

#include "chunk.h"
#include "value.h"

typedef struct {
  int capacity;
  Value* array;
  Value* top;
} Stack;

typedef struct {
  Chunk* chunk;
  uint8_t* ip;
  Stack stack;
} VM;

typedef enum {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} InterpretResult;

void initVM();
InterpretResult interpret(const char* source);
void push(Value value);
void setCurrent(Value value);
Value pop();
Value peek(int distance);
void freeVM();

#endif
