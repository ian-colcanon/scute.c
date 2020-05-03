#ifndef scute_obj_def_h
#define scute_obj_def_h

#include <stdio.h>
#include <string.h>

#include "common.h"
#include "memory.h"
#include "hashmap.h"
#include "svg.h"
#include "value.h"
#include "chunk.h"
#include "natives.h"
#include "color.h"
#include "common.h"


typedef enum {
	OBJ_STRING,
	OBJ_SHAPE,
	OBJ_SCOPE,
	OBJ_INST,
	OBJ_CHUNK,
	OBJ_NATIVE,
	OBJ_COLOR,
	OBJ_ARRAY,
} OBJType;

typedef enum {
	INST_NONE,
	INST_SHAPE,
	INST_SEG,
	INST_TRANS,
} InstanceType;

//definition for Obj
struct sObj {
	OBJType type;
	struct sObj* next;
};

//definition for ObjString
struct sObjString{
	Obj object;
	int length;
	char* chars;
	uint32_t hash;
};

struct sObjArray{
	Obj object;
	ValueArray* array;
};

struct sObjInstance{
	Obj object;
	HashMap* map;
	InstanceType type;
	TKType subtype;
};

struct sObjChunk{
	Obj object;
	Chunk* chunk;
	ObjString* funcName;
	CKType chunkType;
	TKType instanceType;
	int numParameters;
	ObjChunk* superChunk;
};

struct sObjNative {
	Obj object;
	NativeFn function;
};

struct sObjShape {
	ObjInstance instance;
	struct sObjShape* nextShape;
	TKType shapeType;

	struct sObjInstance** segments;
	struct sObjInstance** transforms;
	int numTransforms;
	int transformCapacity;

	int numSegments;
	int segmentCapacity;
};

#endif