#ifndef scute_package_h
#define scute_package_h
#include "common.h"
#include "chunk.h"
#include "hashmap.h"
#include "value.h"

typedef struct {
	InterpretResult result;
	Obj* objects;
	Chunk* compiled;
	int lowerLimit;
	int upperLimit;
	HashMap* strings;
} CompilePackage;

void freeCompilationPackage(CompilePackage* code);
CompilePackage* initCompilationPackage();

#endif

