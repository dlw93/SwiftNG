#include "hashmap.h"

#define GROWTH_FACTOR 2

void hashmap_init(THashMap * map, size_t capacity, FHash hash, FCmp cmp) {
	map->hash = hash;
	map->cmp = cmp;
	map->items_count = 0;
	map->capacity = (capacity + 1) << 1;	// prohibit "calloc(0)"
	map->items = calloc(map->capacity, sizeof(TSlot));
}

void hashmap_destroy(THashMap *map) {
	free(map->items);
}

TSlot *find_slot_by_hash(THashMap * map, size_t hash) {
	size_t pos = hash % map->capacity;
	size_t j = 0;
	TSlot *slot = map->items + pos;

	for (size_t i = 0; i < map->capacity; i++) {
		if (slot->hash == hash || !slot->taken) {
			return slot;
		}
		else {
			j += j + 1;	// quadratic probing: this is the same as probe(j)=j+j^2

			size_t p = (pos + j) % map->capacity;
			slot = map->items + p;
		}
	}

	return NULL;
}

TSlot *find_slot(THashMap * map, void * key) {
	size_t hash = map->hash(key);

	return find_slot_by_hash(map, hash);
}

void set_by_hash(THashMap * map, size_t hash, void * val) {
	TSlot *slot = find_slot_by_hash(map, hash);

	slot->taken = true;
	slot->hash = hash;
	slot->val = val;
}

void expand(THashMap *map) {
	TSlot *items_old = map->items;
	size_t capacity_old = map->capacity;

	map->capacity *= GROWTH_FACTOR;
	map->items = calloc(map->capacity, sizeof(TSlot));

	for (size_t i = 0; i < capacity_old; i++) {
		TSlot slot = items_old[i];

		set_by_hash(map, slot.hash, slot.val);
	}

	free(items_old);
}

void hashmap_set(THashMap * map, void * key, void * val) {
	if (map->items_count > (map->capacity >> 1)) {	// expand, if more then half full
		expand(map);
	}

	TSlot *slot = find_slot(map, key);

	if (!slot->taken) {
		map->items_count++;	// do only when slot was empty
	}

	slot->taken = true;
	slot->hash = map->hash(key);
	slot->val = val;
}

void * hashmap_get(THashMap * map, void * key) {
	TSlot *slot = find_slot(map, key);

	return slot != NULL ? slot->val : NULL;
}

int hashmap_contains(THashMap * map, void * key) {
	return hashmap_get(map, key) != NULL;
}
