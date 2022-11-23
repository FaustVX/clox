#include "vm.h"
#include <stdio.h>
#include "debug.h"
#include "common.h"
#include "memory.h"

VM vm;

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define READ_LONG_CONSTANT() (vm.chunk->constants.values[READ_BYTE() | (READ_BYTE() << 8) | (READ_BYTE() << 16)])
#define BINARY_OP(op) \
    do { \
      double b = pop(); \
      double a = pop(); \
      push(a op b); \
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
      case OP_ADD: {
        BINARY_OP(+);
        break;
      }
      case OP_SUBTRACT: {
        BINARY_OP(-);
        break;
      }
      case OP_MULTIPLY: {
        BINARY_OP(*);
        break;
      }
      case OP_DIVIDE: {
        BINARY_OP(/);
        break;
      }
      case OP_NEGATE: {
        push(-pop());
        break;
      }
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

static void resetStack() {
  vm.stack.top = vm.stack.array;
}

void initVM() {
  vm.stack.capacity = 256;
  vm.stack.array = GROW_ARRAY(Value, vm.stack.array, 0, vm.stack.capacity);
  resetStack();
}

InterpretResult interpret(Chunk* chunk) {
  vm.chunk = chunk;
  vm.ip = vm.chunk->code;
  return run();
}

void push(Value value) {
  *vm.stack.top = value;
  size_t count = vm.stack.top - vm.stack.array + 1;
  if (count >= vm.stack.capacity)
  {
    int oldCapacity = vm.stack.capacity;
    vm.stack.capacity *= 2;
    vm.stack.array = GROW_ARRAY(Value, vm.stack.array, oldCapacity, vm.stack.capacity);
    vm.stack.top = vm.stack.array + count;
  }
  else
    vm.stack.top++;
}

Value pop() {
  vm.stack.top--;
  return *vm.stack.top;
}

void freeVM() {
  vm.stack.array = GROW_ARRAY(Value, vm.stack.array, vm.stack.capacity, 0);
  vm.stack.capacity = 0;
}
