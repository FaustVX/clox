#include "common.h"
#include "chunk.h"
#include "debug.h"

int main(int argc, const char* argv[]) {
  Chunk chunk;
  initChunk(&chunk);

  writeChunk(&chunk, OP_CONSTANT, 1);
  writeChunk(&chunk, addConstant(&chunk, 1.2), 1);

  writeChunk(&chunk, OP_RETURN, 1);
  disassembleChunk(&chunk, "test chunk");
  freeChunk(&chunk);
  return 0;
}
