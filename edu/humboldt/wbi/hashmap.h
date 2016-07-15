#ifndef GRAPHS_HASHMAP_H
#define GRAPHS_HASHMAP_H

#include <stdlib.h>

typedef struct HashMap THashMap;
typedef struct Slot TSlot;
typedef size_t(*hash_fn)(void* key);

struct Slot {
	size_t hash;
	void *val;
};

struct HashMap {
	hash_fn hash;
	size_t items_count;
	size_t capacity;
	TSlot * items;
};

void hashmap_init(THashMap *map, size_t capacity, hash_fn hash);

void hashmap_set(THashMap *map, void *key, void *val);

void *hashmap_get(THashMap *map, void *key);

int hashmap_contains(THashMap *map, void *key);

#endif //GRAPHS_HASHMAP_H
