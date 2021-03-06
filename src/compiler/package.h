#ifndef scute_package_h
#define scute_package_h
#include "common.h"
#include "chunk.h"
#include "hashmap.h"
#include "value.h"
struct sIntermediate{
	OpCode operator;
	double value;
};

typedef struct {
	InterpretResult result;
	
	Obj* objects;
	ObjChunk* compiled;
	
	ObjAnim** animations;
	int numAnimations;
	int animCapacity;

	int lowerLimit;
	int upperLimit;

	HashMap* strings;
	HashMap* globals;

} CompilePackage;

void freeCompilationPackage(CompilePackage* code);
CompilePackage* initCompilationPackage();
void freeObjects(Obj* list);
void initCompilingChunk(CompilePackage* package);
void addAnimation(CompilePackage* package, ObjAnim* anim);
#endif

