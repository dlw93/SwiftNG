#include "hashmap.h"

void hashmap_init(THashMap * map, size_t capacity, hash_fn hash) {
	map->items = calloc(capacity * 3, sizeof(TSlot));	// load factor shall be less then 0.319; 1 / 0.319 ~ 3.315 ~ 3
	map->items_count = 0;
	map->capacity = capacity * 3;
	map->hash = hash;
}

// TODO: rehash, resize

TSlot *find_slot(THashMap * map, void * key) {
	size_t hash = map->hash(key);

	size_t pos = hash % map->capacity;
	TSlot *slot = map->items + pos;

	while (slot->val != NULL && slot->hash != hash) {
		pos = (pos + 1) % map->capacity;	// we use linear probing (as of now) as this turned out to be way faster in our case as compared to double hashing
		slot = map->items + pos;
	}

	return slot;
}

void hashmap_set(THashMap * map, void * key, void * val) {
	TSlot *slot = find_slot(map, key);

	slot->hash = map->hash(key);
	slot->val = val;

	map->items_count++;	// do only when slot was empty
}

void * hashmap_get(THashMap * map, void * key) {
	return find_slot(map, key)->val;
}

int hashmap_contains(THashMap * map, void * key) {
	return find_slot(map, key)->val != NULL;
}
