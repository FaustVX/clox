#include <stdlib.h>

#include "chunk.h"
#include "memory.h"

void initLinesArray(LinesArray* array) {
  array->lines = NULL;
  array->capacity = 0;
  array->count = 0;
}

void freeLinesArray(LinesArray* array){
  FREE_ARRAY(Lines, array->lines, array->capacity);
  initLinesArray(array);
}

void addLine(LinesArray* array, int line) {
  if (array->capacity < array->count + 1) {
    int oldCapacity = array->capacity;
    array->capacity = GROW_CAPACITY(oldCapacity);
    array->lines = GROW_ARRAY(Lines, array->lines, oldCapacity, array->capacity);
  }
  if (array->count > 0 && array->lines[array->count - 1].line == line)
  {
    array->lines[array->count - 1].count++;
    return;
  }
  Lines lines = { .count = 1, .line = line };
  array->lines[array->count] = lines;
  array->count++;
}

void initChunk(Chunk* chunk) {
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->code = NULL;
  initLinesArray(&chunk->lines);
  initValueArray(&chunk->constants);
}

void freeChunk(Chunk* chunk){
  FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
  freeLinesArray(&chunk->lines);
  freeValueArray(&chunk->constants);
  initChunk(chunk);
}

void writeChunk(Chunk* chunk, uint8_t byte, int line) {
  if (chunk->capacity < chunk->count + 1) {
    int oldCapacity = chunk->capacity;
    chunk->capacity = GROW_CAPACITY(oldCapacity);
    chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
  }
  chunk->code[chunk->count] = byte;
  chunk->count++;
  addLine(&chunk->lines, line);
}

void writeConstant(Chunk* chunk, Value value, int line) {
  int constant = addConstant(chunk, value);
  if (constant < 256) {
    writeChunk(chunk, OP_CONSTANT, line);
    writeChunk(chunk, constant, line);
  }
  else if (constant < 16777216) {
    
    writeChunk(chunk, OP_LONG_CONSTANT, line);
    writeChunk(chunk, constant, line);
    writeChunk(chunk, constant >> 8, line);
    writeChunk(chunk, constant >> 16, line);
  }
  else {
    exit(1);
  }
}

int addConstant(Chunk* chunk, Value value) {
  writeValueArray(&chunk->constants, value);
  return chunk->constants.count - 1;
}


int getLine(LinesArray* array, int offset) {
  int index = 0;
  Lines line = array->lines[index];
  uint8_t count = line.count;
  for (size_t i = 0; i < offset; i++)
    if (--count == 0) {
      line = array->lines[++index];
      count = line.count;
    }
  return line.line;
}
