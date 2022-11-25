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

ObjString* concatString(ObjString* a, ObjString* b) {
  int length = a->length + b->length;
  ObjString* string = allocateString(length);
  memcpy(string->chars, a->chars, a->length);
  memcpy(string->chars + a->length, b->chars, b->length);
  return string;
}

ObjString* copyString(const char* chars, int length) {
  ObjString* string = allocateString(length);
  memcpy(string->chars, chars, length);
  return string;
}

void printObject(Value value) {
  switch (OBJ_TYPE(value)) {
    case OBJ_STRING:
      printf("%s", AS_CSTRING(value));
      break;
  }
}
