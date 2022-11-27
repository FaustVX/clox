#ifndef clox_vm_h
#define clox_vm_h

#include "chunk.h"
#include "value.h"
#include "table.h"
#include "object.h"

#define FRAMES_MAX 64

typedef struct {
  ObjFunction* function;
  uint8_t* ip;
  Value* slots;
} CallFrame;

typedef struct {
  int capacity;
  Value* array;
  Value* top;
} Stack;

typedef struct {
  CallFrame frames[FRAMES_MAX];
  int frameCount;
  Chunk* chunk;
  uint8_t* ip;
  Stack stack;
  Table globals;
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
