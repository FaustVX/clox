#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

int main(int argc, const char* argv[]) {
  initVM();
  Chunk chunk;
  initChunk(&chunk);
  writeConstant(&chunk, 1.2, 1);
  writeChunk(&chunk, OP_RETURN, 1);
  disassembleChunk(&chunk, "test chunk");
  interpret(&chunk);
  freeChunk(&chunk);
  freeVM();
  return 0;
}
