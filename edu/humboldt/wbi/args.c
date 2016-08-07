#include "args.h"
#include <string.h>

void args_init(TArgs * args, int argc, char * argv[]) {
	hashmap_init(args, argc - 1, &jenkins_oat_hash, &strcmp);

	for (size_t i = 1; i < argc; i++) {
		char *arg = argv[i] + 2;
		char *pos = strchr(arg, '=');

		if (pos) {
			*pos = 0;
			hashmap_set(args, arg, pos + 1);
		}
		else {
			hashmap_set(args, arg, "");
		}
	}
}

bool args_has(TArgs * args, char * key) {
	return hashmap_contains(args, key);
}

char *args_get(TArgs * args, char * key) {
	return hashmap_get(args, key);
}
