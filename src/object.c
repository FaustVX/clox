#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"
#include "table.h"

#define ALLOCATE_OBJ(type, objectType) \
    (type*)allocateObject(sizeof(type), objectType)

static Obj* allocateObject(size_t size, ObjType type) {
  Obj* object = (Obj*)reallocate(NULL, 0, size);
  object->type = type;
  object->next = vm.objects;
  vm.objects = object;
  return object;
}

ObjFunction* newFunction() {
  ObjFunction* function = ALLOCATE_OBJ(ObjFunction, OBJ_FUNCTION);
  function->arity = 0;
  function->name = NULL;
  initChunk(&function->chunk);
  return function;
}

static ObjString* allocateString(int length, uint32_t hash) {
  ObjString* string = (ObjString*)allocateObject(sizeof(ObjString) + length + 1, OBJ_STRING);
  string->chars[length] = '\0';
  string->length = length;
  string->hash = hash;
  tableSet(&vm.strings, string, NIL_VAL);
  return string;
}

static uint32_t hashString(const char* key, int length) {
  uint32_t hash = 2166136261u;
  for (int i = 0; i < length; i++) {
    hash ^= (uint8_t)key[i];
    hash *= 16777619;
  }
  return hash;
}

ObjString* concatString(ObjString* a, ObjString* b) {
  return concatStringRaw(a->chars, a->length, b->chars, b->length);
}

ObjString* concatStringRaw(char* a, int lenA, char* b, int lenB) {
  int length = lenA + lenB;
  char* chars = ALLOCATE(char, length + 1);
  memcpy(chars, a, lenA);
  memcpy(chars + lenA, b, lenB);
  chars[length] = '\0';
  uint32_t hash = hashString(chars, length);
  ObjString* interned = tableFindString(&vm.strings, chars, length, hash);
  if (interned != NULL) {
    FREE_ARRAY(char, chars, length + 1);
    return interned;
  }
  ObjString* string = allocateString(length, hash);
  memcpy(string->chars, chars, length);
  FREE_ARRAY(char, chars, length + 1);

  return string;
}

ObjString* copyString(const char* chars, int length) {
  uint32_t hash = hashString(chars, length);
  ObjString* interned = tableFindString(&vm.strings, chars, length, hash);
  if (interned != NULL)
    return interned;
  ObjString* string = allocateString(length, hash);
  memcpy(string->chars, chars, length);
  return string;
}

static void printFunction(ObjFunction* function) {
  printf("<fn %s>", function->name->chars);
}

void printObject(Value value) {
  switch (OBJ_TYPE(value)) {
    case OBJ_STRING:
      printf("%s", AS_CSTRING(value));
      break;
    case OBJ_FUNCTION:
      printFunction(AS_FUNCTION(value));
      break;
  }
}
