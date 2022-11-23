#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

typedef enum {
  OP_CONSTANT,
  OP_LONG_CONSTANT,
  OP_RETURN,
} OpCode;

typedef struct {
  int line;
  uint8_t count;
} Lines;

typedef struct {
  int count;
  int capacity;
  Lines* lines;
} LinesArray;

typedef struct {
  int count;
  int capacity;
  uint8_t* code;
  LinesArray lines;
  ValueArray constants;
} Chunk;

void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);
void writeConstant(Chunk* chunk, Value value, int line);
int addConstant(Chunk* chunk, Value value);
int getLine(LinesArray* array, int offset);

#endif
