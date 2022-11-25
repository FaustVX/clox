#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, objectType) \
    (type*)allocateObject(sizeof(type), objectType)

static Obj* allocateObject(size_t size, ObjType type) {
  Obj* object = (Obj*)reallocate(NULL, 0, size);
  object->type = type;
  object->next = vm.objects;
  vm.objects = object;
  return object;
}

static ObjString* allocateString(int length) {
  ObjString* string = (ObjString*)allocateObject(sizeof(ObjString) + length + 1, OBJ_STRING);
  string->chars[length] = '\0';
  string->length = length;
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
  ObjString* string = allocateString(length);
  memcpy(string->chars, a, lenA);
  memcpy(string->chars + lenA, b, lenB);
  string->hash = hashString(string->chars, string->length);
  return string;
}

ObjString* copyString(const char* chars, int length) {
  ObjString* string = allocateString(length);
  memcpy(string->chars, chars, length);
  string->hash = hashString(string->chars, string->length);
  return string;
}

void printObject(Value value) {
  switch (OBJ_TYPE(value)) {
    case OBJ_STRING:
      printf("%s", AS_CSTRING(value));
      break;
  }
}
