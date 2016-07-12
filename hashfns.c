#include "hashfns.h"

size_t jenkins_oat_hash(void * key) {
	size_t hash = 0;

	for (char *c = (char*) key; *c != 0; c++) {
		hash += *c;
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}

	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);

	return hash;
}

size_t fnv_hash(void * key) {
	size_t hash = 2166136261;

	for (unsigned char *c = (unsigned char*) key; *c != 0; c++) {
		hash = (hash * 16777619) ^ *c;
	}

	return hash;
}
