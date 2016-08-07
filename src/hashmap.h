#ifndef GRAPHS_HASHMAP_H
#define GRAPHS_HASHMAP_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct HashMap THashMap;
typedef struct Slot TSlot;
typedef size_t(*FHash)(void* key);
typedef bool(*FCmp)(void* key1, void* key2);

struct Slot {
	bool taken;
	size_t hash;
	void *key;
	void *val;
};

struct HashMap {
	FHash hash;
	FCmp cmp;
	size_t items_count;
	size_t capacity;
	TSlot * items;
};

void hashmap_init(THashMap *map, size_t capacity, FHash hash, FCmp cmp);

void hashmap_destroy(THashMap *map);

void hashmap_set(THashMap *map, void *key, void *val);

void *hashmap_get(THashMap *map, void *key);

int hashmap_contains(THashMap *map, void *key);

#endif //GRAPHS_HASHMAP_H
