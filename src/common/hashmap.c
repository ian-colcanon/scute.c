#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "hashmap.h"
#include "value.h"
#include "memory.h"
#include "obj.h"
#include "output.h"

void initMap(HashMap** map){
	*map = ALLOCATE(HashMap, 1);
	(*map)->numEntries = 0;
	(*map)->capacity = 0;
	(*map)->entries = NULL;
	(*map)->first = NULL;
	(*map)->previous = NULL;
} 

static bool isDeleted(HashEntry* entry){
	return entry->key == NULL && AS_BOOL(entry->value);
}

void grow(HashMap* map){
	HashEntry* rehashed = NULL;
	int oldCapacity = map->capacity;
	map->capacity = GROW_CAPACITY(map->capacity);
	rehashed = GROW_ARRAY(rehashed, HashEntry, 0, map->capacity);

	for(int i = 0; i < map->capacity; ++i){
		rehashed[i].key = NULL;
		rehashed[i].value = NULL_VAL();
	}
	HashEntry* current = map->first;
	while(current != NULL){
		insert(map, current->key, current->value);
		current = current->next;
	}
	FREE_ARRAY(HashEntry, map->entries, oldCapacity);
	map->entries = rehashed;
}

void freeMap(HashMap* map){
	FREE_ARRAY(HashEntry, map->entries, map->capacity);
}

uint32_t hashFunction(char* keyString, int length){
	uint32_t hash = 2166136261u;

	for(int i = 0; i<length; ++i){
		hash ^= keyString[i];
		hash *= 16777619;
	}
	return hash;
}

int hashIndex(HashMap* map, ObjString* obj){
	return obj->hash % map->capacity;
}

static HashEntry* includes(HashMap* map, ObjString* key){
	if(map->capacity == 0) grow(map);
	int index = hashIndex(map, key);
	if(map->numEntries == 0) return &(map->entries[index]);

	while(map->entries[index].key != NULL){
		if(map->entries[index].key == key) return &(map->entries[index]);
		index = (index + 1) % map->capacity;
	}
	return &(map->entries[index]);
}

HashEntry* insert(HashMap* map, ObjString* key, Value value){
	if(map->numEntries + 1 > map->capacity) grow(map);
	HashEntry* spot = includes(map, key);

	spot->key = key;
	spot->value = value;
	spot->next = NULL;

	if(map->numEntries == 0) {
		map->first = spot;
		map->previous = map->first;
	}else{
		map->previous->next = spot;
		map->previous = spot;
	}
	++map->numEntries;
	return spot;
}

Value getValue(HashMap* map, ObjString* key){
	HashEntry* entry = includes(map, key);
	if(entry->key != NULL){
		return entry->value;
	}else{
		return NULL_VAL();
	}
}

//implement to return the entire HashEntry
ObjString* findKey(HashMap* map, char* chars, int length){
	if(map->capacity == 0 || map->numEntries == 0) return NULL;

	uint32_t hash = hashFunction(chars, length);
	uint32_t index = hash % map->capacity;

	while(map->entries[index].key != NULL ){
		HashEntry current = map->entries[index];

		if(!isDeleted(&current) && hash == current.key->hash){
			int result = (int) memcmp(current.key->chars, chars, length);
			if(result == 0) {
				return current.key;
			}
		}
		
		index = (index + 1) % map->capacity;
	}
	return NULL;
}

void set(HashMap* map, ObjString* key, Value value) {
	HashEntry* entry = includes(map, key);
	if(entry != NULL){
		entry->value = value;
	}
}

void delete(HashMap* map, ObjString* key){
	HashEntry* entry = includes(map, key);
	if(entry != NULL){
		entry->key = NULL;
		entry->value = BOOL_VAL(true);
	}
}

void printMap(OutType out, HashMap* map, int indents){
	HashEntry* first = map->first;

	while(first != NULL){
		for(int i = 0; i<indents; ++i){
			print(out, "   ");
		}
		print(out, "%s: ", first->key->chars);
		switch(first->value.type){
			case VL_OBJ: {
				Obj* valObj = AS_OBJ(first->value);
				switch(valObj->type){
					case OBJ_SCOPE: {
						print(out, "\n");
						ObjScope* closeObj = (ObjScope*) valObj;
						printMap(out, closeObj->map, indents+1);
						} break;
				}
				} break;
			default:
				print(out, "%g\n", first->value.as.number);
				break;

		}
		first = first->next;
	}
}

static void displayFullMap(HashMap* map){
	print(O_DEBUG, "[");
	if(map->capacity == 0 || map->numEntries == 0) {
		print(O_DEBUG, "]"); 
		return;
	}
	if(map->entries[0].key == NULL){
		print(O_DEBUG, "X");
	}else{
		print(O_DEBUG, "%g", AS_NUM((map->entries[0].value)));
	}

	for(int i = 1; i<map->capacity; ++i){
		if(map->entries[i].key == NULL){
			print(O_DEBUG, ", X");
		}else{
			print(O_DEBUG, ", %g", AS_NUM((map->entries[i].value)));
		}
	}
	print(O_DEBUG, "]\n");
}