#ifndef ARGS_H
#define ARGS_H

#include <stdlib.h>
#include <stdbool.h>
#include "edu/humboldt/wbi/hashmap.h"
#include "hashfns.h"

typedef struct HashMap TArgs;

void args_init(TArgs *args, int argc, char *argv[]);

bool args_has(TArgs *args, char *key);

char *args_get(TArgs *args, char *key);

#endif //ARGS_H
