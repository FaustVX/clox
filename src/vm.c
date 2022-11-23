#include "vm.h"
#include <stdio.h>
#include "debug.h"
#include "common.h"

VM vm;

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define READ_LONG_CONSTANT() (vm.chunk->constants.values[READ_BYTE() | (READ_BYTE() << 8) | (READ_BYTE() << 16)])

  for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
    disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif
    uint8_t instruction;
    switch (instruction = READ_BYTE()) {
      case OP_CONSTANT: {
        Value constant = READ_CONSTANT();
        printValue(constant);
        printf("\n");
        break;
      }
      case OP_LONG_CONSTANT: {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsequence-point"
        Value constant = READ_LONG_CONSTANT();
#pragma GCC diagnostic pop
        printValue(constant);
        printf("\n");
        break;
      }
      case OP_RETURN: {
        return INTERPRET_OK;
      }
    }
  }

#undef READ_LONG_CONSTANT
#undef READ_CONSTANT
#undef READ_BYTE
}

void initVM() {
  
}

InterpretResult interpret(Chunk* chunk) {
  vm.chunk = chunk;
  vm.ip = vm.chunk->code;
  return run();
}

void freeVM() {
  
}
