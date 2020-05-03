#include <stdio.h>
#include <stdlib.h>
#include "svg.h"
#include "hashmap.h"
#include "obj.h"
#include "obj_def.h"
#include "value.h"
#include "vm.h"
#include "scanner.h"
#include "color.h"
#include "math.h"
#include "package.h"



#ifdef EM_MAIN

void resolveColor(const char* key, Value val){
	if(IS_OBJ(val)){
		Obj* objVal = AS_OBJ(val);
		switch(objVal->type){
			case OBJ_ARRAY: ;
		 		ValueArray* vArray = AS_ARRAY(val)->array;
				em_addColorStyle(key, vArray->count, vArray->values);
				break;
			default:
				break;
		}
	}else{
		if(IS_NUM(val)){
			
		}
	}
}

void assignStyles(ObjShape* shape){
	HashMap* shapeMap = shape->instance.map;
	Value strokeWidth = getValue(shapeMap, string("strokeWidth"));
	em_addStyle("strokeWidth", &strokeWidth);

	Value fill = getValue(shapeMap, string("fill"));
	resolveColor("fill", fill);

	Value stroke = getValue(shapeMap, string("stroke"));
	resolveColor("stroke", stroke);
}
#endif

void drawShape(ObjShape* shape, ValueLink* links){
	HashMap* shapeMap = shape->instance.map;
	TKType type = shape->shapeType;
	#ifdef EM_MAIN
		unsigned address = (unsigned) shapeMap;
		em_newShape(address, type);
		
		switch(type){
			case TK_RECT: { 
				Value posVal = getValue(shapeMap, string("position"));
				Value sizeVal = getValue(shapeMap, string("size"));
				Value roundingVal = getValue(shapeMap, string("rounding"));

				ValueArray* sizeArray = AS_ARRAY(sizeVal)->array;
				ValueArray* posArray = AS_ARRAY(posVal)->array;
				
				em_addVectorAttr("size", sizeArray->values);
				em_addVectorAttr("position", posArray->values);
				em_addAttribute("rounding", &roundingVal);
			} break;

			case TK_CIRC:{
				Value posVal = getValue(shapeMap, string("position"));
				Value radVal = getValue(shapeMap, string("radius"));

				ValueArray* posArray = AS_ARRAY(posVal)->array;
				
				em_addVectorAttr("position", posArray->values);
				em_addAttribute("radius", &radVal);
				} break;

			case TK_ELLIP: {
				Value posVal = getValue(shapeMap, string("position"));
				Value radVal = getValue(shapeMap, string("radius"));

				ValueArray* posArray = AS_ARRAY(posVal)->array;
				ValueArray* radArray = AS_ARRAY(radVal)->array;

				em_addVectorAttr("position", posArray->values);
				em_addVectorAttr("radius", posArray->values);
				} break;

			case TK_LINE: {
				Value startVal = getValue(shapeMap, string("start"));
				Value endVal = getValue(shapeMap, string("end"));

				ValueArray* startArray = AS_ARRAY(startVal)->array;
				ValueArray* endArray = AS_ARRAY(endVal)->array;

				em_addVectorAttr("start", startArray->values);
				em_addVectorAttr("end", endArray->values);
				} break;
			
			default:
				break;
		}
		em_paintShape();
		#else	
			printMap(O_OUT, shapeMap, 0);
		#endif
}

double toRadians(int degrees){
	return (PI/180) * degrees;
}


void processTransformations(ObjShape* shape){
	for(int i = 0; i<shape->numTransforms; ++ i){

		ObjInstance* transform = shape->transforms[i];
		
		switch(transform->subtype){
			case TK_TRANS: {
				Value vectorValue = getValue(transform->map, string("vector"));
				ValueArray* vector = AS_ARRAY(vectorValue)->array;
				#ifdef EM_MAIN
					em_addTranslate(vector->values);
				#endif
				print(O_OUT, "transform: ");
				printValue(O_OUT, vectorValue);
				print(O_OUT, "\n");
			}break;
			default:
				break;
		}
	}

}


void drawPoints(ObjShape* shape, ValueLink* link){
	unsigned address = (unsigned) shape->instance.map;
	#ifdef EM_MAIN
		em_newShape(address, shape->shapeType);
	#endif

	int angle = 0;
	int points[2];
	points[0] = 0;
	points[1] = 1;

	for(int i = 0; i<shape->numSegments; ++i){
		ObjInstance* segment = shape->segments[i];
		HashMap* map = segment->map;

		switch(segment->subtype){
			case TK_JUMP: {
				ObjArray* vector = AS_ARRAY(getValue(map, string("position")));
				points[0] = AS_NUM(getValueArray(vector->array, 0));
				points[1] = AS_NUM(getValueArray(vector->array, 1));
				#ifdef EM_MAIN
					em_addJump(vector->array->values);
				#else
					print(O_OUT, "Jump: (%d, %d)\n", points[0], points[1]);
				#endif
			} break;
			case TK_VERT: ;
				ObjArray* vector = AS_ARRAY(getValue(map, string("position")));
				points[0] = AS_NUM(getValueArray(vector->array, 0));
				points[1] = AS_NUM(getValueArray(vector->array, 1));
				#ifdef EM_MAIN
					em_addVertex(vector->array->values);
				#else
					print(O_OUT, "Vertex: (%d, %d)\n", points[0], points[1]);
				#endif
				break;
			case TK_MOVE: ;
				Value distance = getValue(map, string("distance"));
				points[0] += (int) round(cos(toRadians(angle))*AS_NUM(distance));
				points[1] += (int) round(sin(toRadians(angle))*AS_NUM(distance));
				#ifdef EM_MAIN
					em_addMove(points[0], points[1], &distance);				
				#else
					print(O_OUT, "Move %f: (%d, %d)\n", AS_NUM(distance), points[0], points[1]);
				#endif
				break;
			case TK_TURN: ;
				Value degrees = getValue(map, string("degrees"));
				#ifdef EM_MAIN
					em_addTurn(&degrees, angle);				
				#else
					print(O_OUT, "Turn by %d deg to %d deg\n", degrees, angle);
				#endif
				angle += AS_NUM(degrees);
				break;
			case TK_QBEZ: {
				Value control = getValue(map, string("control"));
				Value end = getValue(map, string("end"));

				Value* controlArray = AS_ARRAY(control)->array->values;
				Value* endArray = AS_ARRAY(end)->array->values;
				#ifdef EM_MAIN
					em_addQuadBezier(controlArray, endArray);
				#else
					print(O_OUT, "qBezier ");
					printValue(O_OUT, control);
					print(O_OUT, " ");
					printValue(O_OUT, end);
					print(O_OUT, "\n");
				#endif
			} break;
			case TK_CBEZ: {
				Value control1 = getValue(map, string("control1"));
				Value control2 = getValue(map, string("control2"));
				Value end = getValue(map, string("end"));

				Value* control1Array = AS_ARRAY(control1)->array->values;
				Value* control2Array = AS_ARRAY(control2)->array->values;
				Value* endArray = AS_ARRAY(end)->array->values;
				#ifdef EM_MAIN
					em_addCubicBezier(control1Array, control2Array, endArray);
				#else
					print(O_OUT, "cBezier ");
					printValue(O_OUT, control1);
					print(O_OUT, " ");
					printValue(O_OUT, control2);
					print(O_OUT, " ");
					printValue(O_OUT, end);
					print(O_OUT, "\n");
				#endif
			} break;
			case TK_ARC: {
				Value center = getValue(map, string("center"));
				Value* centerArray = AS_ARRAY(center)->array->values;

				Value degrees = getValue(map, string("degrees"));
				Value* degreesArray = AS_ARRAY(degrees)->array->values;

				Value radius = getValue(map, string("radius"));
				Value* radiusArray = AS_ARRAY(radius)->array->values;

				#ifdef EM_MAIN
					em_addArc(centerArray, radiusArray, degreesArray);
				#else
					print(O_OUT, "arc ");
					printValue(O_OUT, center);
					print(O_OUT, " ");
					printValue(O_OUT, degrees);
					print(O_OUT, " ");
					printValue(O_OUT, radius);
					print(O_OUT, "\n");
				#endif
			} break;
			default:
				break;
		}
	}
	#ifdef EM_MAIN
		em_paintShape();
	#endif
}



ObjShape* popShape(){
	ObjShape* latestShape = vm.shapeStack[vm.shapeCount-1];
	--vm.shapeCount;
	return latestShape;
}

void pushShape(ObjShape* close){
	if(vm.shapeCount + 1 > vm.shapeCapacity){
		int oldCapacity = vm.shapeCapacity;
		vm.shapeCapacity = GROW_CAPACITY(oldCapacity);
		vm.shapeStack = GROW_ARRAY(vm.shapeStack, ObjShape*, oldCapacity, vm.shapeCapacity);
	}
	vm.shapeStack[vm.shapeCount] = close;
	++vm.shapeCount;
}

void renderFrame(CompilePackage* code){
	while(vm.shapeCount > 0){
		ObjShape* top = popShape();
		#ifdef EM_MAIN
			assignStyles(top);
		#endif
		processTransformations(top);
		switch(top->shapeType){
			case TK_POLY:
			case TK_POLYL:
			case TK_PATH:
				drawPoints(top, code->links);
				break;
			default:
				drawShape(top, code->links);	
				break;
		}
	}
}