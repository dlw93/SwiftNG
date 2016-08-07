//
// Created by david on 12/18/2015.
//

#include <stdlib.h>
#include <stdio.h>
#include "treeset.h"

void treeset_init(TTreeSet *t) {
    t->root = 0;
}

void treeset_add(TTreeSet *t, void *entry, int key) {
    TTreeNode** node = &t->root;

    do {
        if (*node == 0) {
            *node = calloc(1, sizeof(TTreeNode));
        }

        node = &(*node)->children[key % N];
        key = key / N;
    } while (key > 0);

    if (*node == 0) {
        *node = calloc(1, sizeof(TTreeNode));
    }

    (*node)->value = entry;
}

TTreeNode *treeset_find(TTreeSet *t, int key) {
    TTreeNode* node = t->root;

    do {
        node = node->children[key % N];
        key = key / N;
    } while (key > 0 && node != NULL);

    return node;
}

int treeset_has(TTreeSet *t, int key) {
    return treeset_find(t, key) != NULL;
}

void *treeset_get(TTreeSet *t, int key) {
	TTreeNode *result = treeset_find(t, key);

    return result != NULL ? result->value : NULL;
}
