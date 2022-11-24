#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "vm.h"
#include "debug.h"
#include "common.h"
#include "memory.h"
#include "compiler.h"
#include "object.h"

VM vm;

static void resetStack() {
  vm.stack.top = vm.stack.array;
}

static bool isFalsey(Value value) {
  return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static void concatenate() {
  ObjString* b = AS_STRING(pop());
  ObjString* a = AS_STRING(peek(0));

  int length = a->length + b->length;
  char* chars = ALLOCATE(char, length + 1);
  memcpy(chars, a->chars, a->length);
  memcpy(chars + a->length, b->chars, b->length);
  chars[length] = '\0';

  ObjString* result = takeString(chars, length);
  setCurrent(OBJ_VAL(result));
}

static void runtimeError(const char* format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputs("\n", stderr);

  size_t instruction = vm.ip - vm.chunk->code - 1;
  int line = getLine(&vm.chunk->lines, instruction);
  fprintf(stderr, "[line %d] in script\n", line);
  resetStack();
}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define READ_LONG_CONSTANT() (vm.chunk->constants.values[READ_BYTE() | (READ_BYTE() << 8) | (READ_BYTE() << 16)])
#define BINARY_OP(valueType, op) \
    do { \
      if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
        runtimeError("Operands must be numbers."); \
        return INTERPRET_RUNTIME_ERROR; \
      } \
      double b = AS_NUMBER(pop()); \
      double a = AS_NUMBER(peek(0)); \
      setCurrent(valueType(a op b)); \
    } while (false)

  for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
    printf("          ");
    for (Value* slot = vm.stack.array; slot < vm.stack.top; slot++) {
      printf("[ ");
      printValue(*slot);
      printf(" ]");
    }
    printf("\n");
    disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif
    uint8_t instruction;
    switch (instruction = READ_BYTE()) {
      case OP_CONSTANT: {
        Value constant = READ_CONSTANT();
        push(constant);
        break;
      }
      case OP_LONG_CONSTANT: {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsequence-point"
        Value constant = READ_LONG_CONSTANT();
#pragma GCC diagnostic pop
        push(constant);
        break;
      }
      case OP_NIL:
        push(NIL_VAL);
        break;
      case OP_TRUE:
        push(BOOL_VAL(true));
        break;
      case OP_FALSE:
        push(BOOL_VAL(false));
        break;
      case OP_EQUAL: {
        Value b = pop();
        Value a = peek(0);
        setCurrent(BOOL_VAL(valuesEqual(a, b)));
        break;
      }
      case OP_GREATER:
        BINARY_OP(BOOL_VAL, >);
        break;
      case OP_LESS:
        BINARY_OP(BOOL_VAL, <);
        break;
      case OP_ADD: {
        if (IS_STRING(peek(0)) && IS_STRING(peek(1))) {
          concatenate();
        } else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1))) {
          double b = AS_NUMBER(pop());
          double a = AS_NUMBER(peek(0));
          setCurrent(NUMBER_VAL(a + b));
        } else {
          runtimeError( "Operands must be two numbers or two strings.");
          return INTERPRET_RUNTIME_ERROR;
        }
        break;
      }
      case OP_SUBTRACT:
        BINARY_OP(NUMBER_VAL, -);
        break;
      case OP_MULTIPLY:
        BINARY_OP(NUMBER_VAL, *);
        break;
      case OP_DIVIDE:
        BINARY_OP(NUMBER_VAL, /);
        break;
      case OP_NOT:
        setCurrent(BOOL_VAL(isFalsey(peek(0))));
        break;
      case OP_NEGATE:
        if (!IS_NUMBER(peek(0))) {
          runtimeError("Operand must be a number.");
          return INTERPRET_RUNTIME_ERROR;
        }
        setCurrent(NUMBER_VAL(-AS_NUMBER(peek(0))));
        break;
      case OP_RETURN: {
        printValue(pop());
        printf("\n");
        return INTERPRET_OK;
      }
    }
  }

#undef BINARY_OP
#undef READ_LONG_CONSTANT
#undef READ_CONSTANT
#undef READ_BYTE
}

void initVM() {
  vm.stack.capacity = 256;
  vm.stack.array = GROW_ARRAY(Value, vm.stack.array, 0, vm.stack.capacity);
  vm.objects = NULL;
  resetStack();
}

InterpretResult interpret(const char* source) {
  Chunk chunk;
  initChunk(&chunk);

  if (!compile(source, &chunk)) {
    freeChunk(&chunk);
    return INTERPRET_COMPILE_ERROR;
  }

  vm.chunk = &chunk;
  vm.ip = vm.chunk->code;

  InterpretResult result = run();

  freeChunk(&chunk);
  return result;
}

void setCurrent(Value value) {
  vm.stack.top[-1] = value;
}

void push(Value value) {
  *vm.stack.top = value;
  size_t count = vm.stack.top - vm.stack.array + 1;
  if (count >= vm.stack.capacity)
  {
    vm.stack.capacity *= 2;
    vm.stack.array = GROW_ARRAY(Value, vm.stack.array, count, vm.stack.capacity);
    vm.stack.top = vm.stack.array + count;
  }
  else
    vm.stack.top++;
}

Value peek(int distance) {
  return vm.stack.top[-1 - distance];
}

Value pop() {
  vm.stack.top--;
  return *vm.stack.top;
}

void freeVM() {
  vm.stack.array = GROW_ARRAY(Value, vm.stack.array, vm.stack.capacity, 0);
  vm.stack.capacity = 0;
  freeObjects();
}
